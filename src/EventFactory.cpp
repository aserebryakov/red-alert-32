#include "EventFactory.h"
#include <ArduinoJson.h>


EventFactory::EventFactory(const String &city) : city{city} {
}


Event EventFactory::createEvent(const String &alerts_json) {
    return NoAlertsEvent{};
}
