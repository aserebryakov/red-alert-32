#ifndef RED_ALERT_32_REDALERTMANAGER_H
#define RED_ALERT_32_REDALERTMANAGER_H

#include <Device.h>
#include <DigitalLed.h>
#include <HwApiImpl.h>
#include <WebServerManager.h>
#include "ConfigurationManager.h"
#include "RedAlertStateMachine.h"

class RedAlertManager : public Device {
    public:
        virtual void begin();
        virtual void loop();

    private:
        static constexpr int GREEN_LED_PIN = 19;
        static constexpr int YELLOW_LED_PIN = 18;
        static constexpr int RED_LED_PIN = 23;

        HwApiImpl hw_api;
        DigitalLed green_led{GREEN_LED_PIN, hw_api};
        DigitalLed yellow_led{YELLOW_LED_PIN, hw_api};
        DigitalLed red_led{RED_LED_PIN, hw_api};

        WebServerManager web_server{};
        ConfigurationManager configuration_manager{};
        RedAlertStateMachine state_machine{};
};


#endif //RED_ALERT_32_REDALERTMANAGER_H