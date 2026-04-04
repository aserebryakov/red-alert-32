#if defined(ARDUINO)
#include <Arduino.h>
#endif

#include "RedAlertManager.h"

RedAlertManager red_alert_manager;

void setup() {
    Serial.begin(11200);
    red_alert_manager.begin();
}

void loop() {
    red_alert_manager.loop();
}
