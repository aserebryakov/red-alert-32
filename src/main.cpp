#include <Arduino.h>
#include "RedAlertManager.h"
#include "WebServerManager.h"
#include <EEPROM.h>

RedAlertManager red_alert_manager;
WebServerManager web_server_manager;

void setup() {
    Serial.begin(9600);

    EEPROM.begin(1024);
    Serial.println("Read from EEPROM");
    const auto read = WebServer::urlDecode(EEPROM.readString(0));
    Serial.println(read);
    Serial.println(EEPROM.read(0));

    red_alert_manager.begin();
    web_server_manager.begin();
}

void loop() {
    web_server_manager.loop();
    Serial.println("Read from EEPROM");
    const auto read = EEPROM.readString(0);
    Serial.println(read);
    Serial.println(EEPROM.read(0));
    red_alert_manager.loop();
}
