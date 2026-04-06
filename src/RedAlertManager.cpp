#include "RedAlertManager.h"
#include <WiFi.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>

String HOST_NAME   = "https://www.oref.org.il";
String PATH_NAME   = "/warningMessages/alert/Alerts.json";

void RedAlertManager::begin() {
    Serial.begin(9600);
    yellow_led.begin();
    green_led.begin();
    red_led.begin();
    configuration_manager.begin();

    const auto configuration = configuration_manager.readConfiguration();

    if (configuration.ssid.value_or("") != "") {
        // Using c_str() just to convert to std::string because String doesn't compile natively but
        // we want to be able to write unit tests
        event_factory.setCity(configuration.cityName.value_or("").c_str());
        WiFi.begin(configuration.ssid.value(), configuration.password.value_or(""));
        Serial.println("Connecting");
        Serial.println("SSID: " + String(configuration.ssid.value()));
        Serial.println("Password: " + String(configuration.password.value_or("")));
        Serial.println("City: " + String(configuration.cityName.value_or("")));

    } else {
        WiFi.begin();
        WiFi.softAP("RED_ALERT_32", "");
        WiFi.softAPConfig({192, 168, 1, 42}, {192, 168, 1, 1}, {255, 255, 255, 0});

        Serial.println(WiFi.softAPIP());
        web_server.begin();
        web_server.setConfigurationCallback([this](const Configuration &config, bool reset) {
            if (reset) {
                this->configuration_manager.reset();
            } else {
                this->configuration_manager.writeConfiguration(config);
            }
            delay(1000);
            ESP.restart();
        });

        if (!MDNS.begin("red-alert-32")) {   // Set the hostname to "esp32.local"
            Serial.println("Error setting up MDNS responder!");
            while(true) {
                delay(1000);
            }
        }
        Serial.println("MDNS responter is running");

        while (true) {
            delay(10);
            web_server.loop();
        }
    }



    constexpr auto CONNECTING_WIFI_INTERVAL_MS{500};
    connecting_wifi_task = scheduler.addPeriodicTask({connectingWifiCallback, this}, CONNECTING_WIFI_INTERVAL_MS);

    constexpr auto CONNECTING_WIFI_BLINK_INTERVAL_MS{200};
    led_task= scheduler.addPeriodicTask({connectingWifiBlinkCallback, this}, CONNECTING_WIFI_BLINK_INTERVAL_MS);
}

void RedAlertManager::loop() {
    constexpr auto LOOP_CYCLE_MS = 10;
    delay(LOOP_CYCLE_MS);
    scheduler.tick(LOOP_CYCLE_MS);
}

void RedAlertManager::requestAlertsJsonCallback(void *context) {
    static_cast<RedAlertManager*>(context)->requestAlertsJson();
}

void RedAlertManager::requestAlertsJson() {
    Serial.println("Requesting alerts...");
    HTTPClient http{};

    http.begin(HOST_NAME + PATH_NAME);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode < 0) {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        return;
    }

    // file found at server
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        return;
    }

    String payload = http.getString();

    // JSON is a little bit broken and has unprintable values before start, so we remove them.
    const auto start = payload.indexOf('{');
    if (start > 0) {
        payload = payload.substring(start);
    }

    // Serial.println(payload);
    const auto event = event_factory.createEvent(payload.c_str());
    Serial.print("Event: ");
    Serial.println(event.index());
    state_machine.processEvent(event);

    http.end();
}

void RedAlertManager::connectingWifiCallback(void *context) {
    static_cast<RedAlertManager*>(context)->connectingWifi();
}

void RedAlertManager::connectingWifi() {
    static int ms_till_reset = 60000;
    if (WiFi.status() != WL_CONNECTED) {
        ms_till_reset -= 500;

        if (ms_till_reset <= 0) {
            Serial.print("Resetting to defaults...");
            configuration_manager.reset();
            ESP.restart();
        }
        Serial.print(".");
        return;
    }

    state_machine.processEvent(WifiConnectedEvent{});
}

void RedAlertManager::httpServerHandleClientCallback(void *context) {
    static_cast<RedAlertManager*>(context)->httpServerHandleClient();
}

void RedAlertManager::httpServerHandleClient() {
    web_server.loop();
}

void RedAlertManager::connectingWifiBlinkCallback(void *context) {
    static_cast<RedAlertManager*>(context)->connectingWifiBlink();
}

void RedAlertManager::connectingWifiBlink() {
    static auto turned_on{false};
    if (turned_on) {
        resetLeds();
    } else {
        red_led.turnOn();
        green_led.turnOn();
    }

    turned_on = !turned_on;
}

void RedAlertManager::stateTransitionCallback(const State &from, const State &to) {
    Serial.print("State transition: ");
    Serial.print(from.index());
    Serial.print(" -> ");
    Serial.println(to.index());

    if (std::holds_alternative<Initialization>(from) && std::holds_alternative<NoAlerts>(to)) {
        Serial.print("Connected to WiFi network with IP Address: ");
        Serial.println(WiFi.localIP());
        scheduler.removeTask(connecting_wifi_task.value());
        scheduler.removeTask(led_task.value());

        web_server.begin();
        web_server.setConfigurationCallback([this](const Configuration &config, bool reset) {
            if (reset) {
                this->configuration_manager.reset();
            } else {
                this->configuration_manager.writeConfiguration(config);
            }
            ESP.restart();
        });

        if (!MDNS.begin("red-alert-32")) {
            Serial.println("Error setting up MDNS responder!");
            while(true) {
                delay(1000);
            }
        }
        Serial.println("MDNS responter is running");

        constexpr auto REQUEST_ALERTS_JSON_INTERVAL_MS{1000};
        alerts_json_request_task_id = scheduler.addPeriodicTask({requestAlertsJsonCallback, this}, REQUEST_ALERTS_JSON_INTERVAL_MS);
        led_task = scheduler.addPeriodicTask({greenLedOnCallback, this}, 1000);
        web_server_task = scheduler.addPeriodicTask({httpServerHandleClientCallback, this}, 10);
        Serial.println("Switched to NoAlerts");
        return;
    }

    if (std::holds_alternative<NoAlerts>(to)) {
        Serial.println("Switched to NoAlerts");
        scheduler.removeTask(led_task.value());
        resetLeds();
        led_task = scheduler.addPeriodicTask({greenLedOnCallback, this}, 1000);
        return;
    }

    if (std::holds_alternative<EarlyWarning>(to)) {
        Serial.println("Switched to EarlyWarning");
        scheduler.removeTask(led_task.value());
        resetLeds();
        led_task = scheduler.addPeriodicTask({redLedOnCallback, this}, 1000);
        return;
    }

    if (std::holds_alternative<YellowAlert>(to)) {
        Serial.println("Switched to YellowAlert");
        scheduler.removeTask(led_task.value());
        resetLeds();
        led_task = scheduler.addPeriodicTask({yellowLedOnCallback, this}, 1000);
        return;
    }

    if (std::holds_alternative<RedAlert>(to)) {
        Serial.println("Switched to RedAlert");
        scheduler.removeTask(led_task.value());
        resetLeds();
        led_task = scheduler.addPeriodicTask({redLedBlinkCallback, this}, 300);
        return;
    }
}

void RedAlertManager::greenLedOnCallback(void *context) {
    static_cast<RedAlertManager*>(context)->greenLedOn();
}

void RedAlertManager::greenLedOn() {
    resetLeds();
    green_led.turnOn();
}

void RedAlertManager::yellowLedOnCallback(void *context) {
    static_cast<RedAlertManager*>(context)->yellowLedOn();
}

void RedAlertManager::yellowLedOn() {
    resetLeds();
    yellow_led.turnOn();
}

void RedAlertManager::redLedOnCallback(void *context) {
    static_cast<RedAlertManager*>(context)->redLedOn();
}

void RedAlertManager::redLedOn() {
    resetLeds();
    red_led.turnOn();
}

void RedAlertManager::redLedBlinkCallback(void *context) {
    static_cast<RedAlertManager*>(context)->redLedBlink();
}

void RedAlertManager::redLedBlink() {
    static auto turned_on{false};
    resetLeds();

    if (turned_on) {
        red_led.turnOn();
    }

    turned_on = !turned_on;
}

void RedAlertManager::resetLeds() const {
    red_led.turnOff();
    green_led.turnOff();
    yellow_led.turnOff();
}
