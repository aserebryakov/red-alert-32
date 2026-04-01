#include "RedAlertManager.h"
#include <WiFi.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>

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
        WiFi.begin(configuration.ssid.value(), configuration.password.value_or(""));
        Serial.println("Connecting");
        Serial.println("SSID: " + String(configuration.ssid.value()));
        Serial.println("Password: " + String(configuration.password.value_or("")));

        int ms_till_reset = 60000;
        while (WiFi.status() != WL_CONNECTED) {
            delay(250);
            red_led.turnOn();
            green_led.turnOn();
            delay(250);
            red_led.turnOff();
            green_led.turnOff();
            Serial.print(".");

            ms_till_reset -= 500;

            if (ms_till_reset <= 0) {
                Serial.print("Resetting to defaults...");
                configuration_manager.reset();
                ESP.restart();
            }
        }

        Serial.println("");
        Serial.print("Connected to WiFi network with IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        WiFi.begin();
        WiFi.softAP("RED_ALERT_32", "");
        WiFi.softAPConfig({192, 168, 1, 42}, {192, 168, 1, 1}, {255, 255, 255, 0});
        Serial.println(WiFi.softAPIP());
    }

    web_server.begin();

    web_server.setConfigurationCallback([this](const Configuration &config, bool reset) {
        if (reset) {
            this->configuration_manager.reset();
        } else {
            this->configuration_manager.writeConfiguration(config);
        }
        ESP.restart();
    });

    if (configuration.ssid.value_or("") == "") {
        while (true) {
            delay(10);
            web_server.loop();
        }
    }
}

void RedAlertManager::loop() {
    delay(1000);
    HTTPClient http;

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

    JsonDocument doc;
    deserializeJson(doc, payload);
    String output;
    serializeJson(doc, output);

    if (doc.isNull()) {
        yellow_led.turnOff();
        green_led.turnOn();
        red_led.turnOff();
    } else {
        yellow_led.turnOn();
        green_led.turnOff();
    }

    if (doc["cat"].as<String>() == "1") {
        Serial.println(doc["cat"].as<String>());
        red_led.turnOn();
    }

    Serial.print(output);

    http.end();
}
