#ifndef RED_ALERT_32_EVENTFACTORY_H
#define RED_ALERT_32_EVENTFACTORY_H

#include <WString.h>

#include "RedAlertStateMachine.h"

class EventFactory {
    public:
        EventFactory(const String& city);
        Event createEvent(const String& alerts_json);

    private:
        String city;
};


#endif //RED_ALERT_32_EVENTFACTORY_H
