#include "EventFactory.h"

constexpr auto RED_ALERT_CATEGORY{"1"};
constexpr auto DRONE_ALERT_CATEGORY{"6"};
constexpr auto EARLY_WARNING_CATEGORY{"14"};
constexpr auto EVENT_ENDED_CATEGORY{"10"};


bool EventFactory::foundCity(JsonDocument& doc) const {
    const auto& array = doc["data"].as<JsonArray>();

    for (const auto &element : array) {
        if (element.template as<std::string>() == city) {
            return true;
        }
    }
    return false;
}


Event EventFactory::handleRedAlert(JsonDocument& doc) const {
    if (foundCity(doc)) {
        return RedAlertEvent{};
    }

    return DistantAlertEvent{};
}


Event EventFactory::handleEarlyWarning(JsonDocument& doc) const {
    if (foundCity(doc)) {
        return EarlyWarningEvent{};
    }

    return NoAlertsEvent{};
}


Event EventFactory::handleEventEnded(JsonDocument& doc) const {
    if (foundCity(doc)) {
        return EventEndedEvent{};
    }

    return NoAlertsEvent{};
}


void EventFactory::setCity(const std::string& _city) {
    city = _city;
}


Event EventFactory::createEvent(const std::string& alerts_json) const {
    if (alerts_json == "error") {
        return ErrorEvent{};
    }

    if (alerts_json == "wifi_connected") {
        return WifiConnectedEvent{};
    }

    JsonDocument doc{};
    deserializeJson(doc, alerts_json);
    const auto alert_category = doc["cat"].as<std::string>();

    if (alert_category == RED_ALERT_CATEGORY || alert_category == DRONE_ALERT_CATEGORY) {
        return handleRedAlert(doc);
    }

    if (alert_category == EARLY_WARNING_CATEGORY) {
        return handleEarlyWarning(doc);
    }

    if (alert_category == EVENT_ENDED_CATEGORY) {
        return handleEventEnded(doc);
    }

    return NoAlertsEvent{};
}
