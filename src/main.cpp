#include <Arduino.h>
#include "RedAlertManager.h"

RedAlertManager red_alert_manager;

void setup() {
    Serial.begin(9600);
    red_alert_manager.begin();
}

void loop() {
    red_alert_manager.loop();
}
