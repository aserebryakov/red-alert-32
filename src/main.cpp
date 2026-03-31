#include <Arduino.h>
#include "RedAlertManager.h"
#include "WebServerManager.h"
#include <EEPROM.h>

RedAlertManager red_alert_manager;
WebServerManager web_server_manager;

void setup() {
    Serial.begin(9600);
    red_alert_manager.begin();
    web_server_manager.begin();
}

void loop() {
    web_server_manager.loop();
    red_alert_manager.loop();
}
