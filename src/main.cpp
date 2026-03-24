#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <CppComponentsLibrary.h>

const char WIFI_SSID[] = "wifi";
const char WIFI_PASSWORD[] = "pwd";

String HOST_NAME   = "https://www.oref.org.il";
String PATH_NAME   = "/warningMessages/alert/Alerts.json";
// String PATH_NAME   = "/warningMessages/alert/History/AlertsHistory.json";

HwApiImpl hw_api;

DigitalLed yellow_led{18, hw_api};
DigitalLed green_led{19, hw_api};

void setup() {
    Serial.begin(9600);
    yellow_led.begin();
    green_led.begin();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    sleep(1);

    HTTPClient http;

    http.begin(HOST_NAME + PATH_NAME);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
        // file found at server
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();

            const auto start = payload.indexOf('{');
            if (start > 0) {
                payload = payload.substring(start);
            }

            JsonDocument doc;
            deserializeJson(doc, payload);
            Serial.print(payload);
            String output;
            serializeJson(doc, output);
            Serial.print(output);
            if (doc.isNull()) {
                yellow_led.turnOff();
                green_led.turnOn();
            }
            else {
                yellow_led.turnOn();
                green_led.turnOff();
            }
        } else {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}
