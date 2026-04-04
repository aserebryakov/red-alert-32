#include <gtest/gtest.h>
#include "EventFactory.h"

class EventFactoryTest: public ::testing::Test {
    protected:
        void SetUp() override {
            under_test.setCity("תל אביב");
        }
        EventFactory under_test{};
};

TEST(EventFactory, Initialization) {
    EventFactory factory{};
}

TEST_F(EventFactoryTest, NoAlertsEvent) {
    const auto event{under_test.createEvent("somegarbage")};
    EXPECT_TRUE(std::holds_alternative<NoAlertsEvent>(event));
}

TEST_F(EventFactoryTest, ErrorEvent) {
    const auto event{under_test.createEvent("error")};
    EXPECT_TRUE(std::holds_alternative<ErrorEvent>(event));
}

TEST_F(EventFactoryTest, WifiConnectedEvent) {
    const auto event{under_test.createEvent("wifi_connected")};
    EXPECT_TRUE(std::holds_alternative<WifiConnectedEvent>(event));
}

TEST_F(EventFactoryTest, DistantAlertEvent) {
    const auto alert_json = R"(
    {
        "cat": "1",
        "data": [
          "יראון"
        ]
    })";
    const auto event{under_test.createEvent(alert_json)};
    EXPECT_TRUE(std::holds_alternative<DistantAlertEvent>(event));
}

TEST_F(EventFactoryTest, RedAlertEvent) {
    const auto alert_json = R"(
    {
        "cat": "1",
        "data": [
          "תל אביב"
        ]
    })";
    const auto event{under_test.createEvent(alert_json)};
    EXPECT_TRUE(std::holds_alternative<RedAlertEvent>(event));
}

TEST_F(EventFactoryTest, EarlyWarningEvent) {
    const auto alert_json = R"(
    {
        "cat": "14",
        "data": [
          "תל אביב"
        ]
    })";
    const auto event{under_test.createEvent(alert_json)};
    EXPECT_TRUE(std::holds_alternative<EarlyWarningEvent>(event));
}

TEST_F(EventFactoryTest, EarlyWarningSomewhereElse) {
    const auto alert_json = R"(
    {
        "cat": "14",
        "data": [
          "יראון",
          "יראון"
        ]
    })";
    const auto event{under_test.createEvent(alert_json)};
    EXPECT_TRUE(std::holds_alternative<NoAlertsEvent>(event));
}

TEST_F(EventFactoryTest, EventEndedSomewhereElse) {
    const auto alert_json = R"(
    {
        "cat": "10",
        "data": [
          "יראון",
          "יראון"
        ]
    })";
    const auto event{under_test.createEvent(alert_json)};
    EXPECT_TRUE(std::holds_alternative<NoAlertsEvent>(event));
}

TEST_F(EventFactoryTest, EventEndedEvent) {
    const auto alert_json = R"(
    {
        "cat": "10",
        "data": [
          "תל אביב"
        ]
    })";
    const auto event{under_test.createEvent(alert_json)};
    EXPECT_TRUE(std::holds_alternative<EventEndedEvent>(event));
}