#include "EventFactory.h"

EventFactory::EventFactory(const std::string &city) : city{city} {
}


bool EventFactory::foundCity(JsonDocument& doc) const {
    const auto& array = doc["data"].as<JsonArray>();

    for (const auto &element : array) {
        if (element.template as<std::string>() == city) {
            return true;
        }
    }
    return false;
}

Event EventFactory::createEvent(const std::string &alerts_json) {
    if (alerts_json == "error") {
        return ErrorEvent{};
    }

    if (alerts_json == "wifi_connected") {
        return WifiConnectedEvent{};
    }

    JsonDocument doc{};
    deserializeJson(doc, alerts_json);

    if (doc["cat"].as<std::string>() == "1") {
        if (foundCity(doc)) {
            return RedAlertEvent{};
        }

        return DistantAlertEvent{};
    }

    if (doc["cat"].as<std::string>() == "14") {
        if (foundCity(doc)) {
            return EarlyWarningEvent{};
        }

        return NoAlertsEvent{};
    }

    if (doc["cat"].as<std::string>() == "10") {
        if (foundCity(doc)) {
            return EventEndedEvent{};
        }

        return NoAlertsEvent{};
    }

    return NoAlertsEvent{};
}