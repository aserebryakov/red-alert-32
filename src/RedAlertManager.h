#ifndef RED_ALERT_32_REDALERTMANAGER_H
#define RED_ALERT_32_REDALERTMANAGER_H

#include <Device.h>
#include <DigitalLed.h>
#include <HwApiImpl.h>
#include <WebServerManager.h>
#include "ConfigurationManager.h"
#include "EventFactory.h"
#include "RedAlertStateMachine.h"
#include "Scheduler.h"

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
        EventFactory event_factory{};
        Scheduler scheduler{};
        std::optional<SchedulerTaskId> alerts_json_request_task_id{};
        std::optional<SchedulerTaskId> connecting_wifi_task{};
        std::optional<SchedulerTaskId> led_task{};

        static void requestAlertsJsonCallback(void *context);
        void requestAlertsJson();

        static void connectingWifiCallback(void *context);
        void connectingWifi();

        static void httpServerHandleClientCallback(void *context);
        void httpServerHandleClient();

        static void connectingWifiBlinkCallback(void *context);
        void connectingWifiBlink();

        void resetLeds() const;
};


#endif //RED_ALERT_32_REDALERTMANAGER_H