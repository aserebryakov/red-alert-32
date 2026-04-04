#ifndef RED_ALERT_32_EVENTFACTORY_H
#define RED_ALERT_32_EVENTFACTORY_H

#include <string>
#include "RedAlertStateMachine.h"
#include <ArduinoJson.h>

class EventFactory {
    public:
        EventFactory(const std::string& city);

        Event createEvent(const std::string& alerts_json);

    private:
        bool foundCity(JsonDocument& doc) const;
        std::string city;
};


#endif //RED_ALERT_32_EVENTFACTORY_H
