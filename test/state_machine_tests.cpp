#include <gtest/gtest.h>
#include "RedAlertStateMachine.h"
#include "RedAlertStateMachine.cpp"

using namespace redalert;

class StateMachineTest : public ::testing::Test {
protected:
    void SetUp() override {
        transitionCount = 0;
        lastOldState = Initialization{};
        lastNewState = Initialization{};
        sm = std::make_unique<RedAlertStateMachine>([this](const State& oldS, const State& newS) {
            transitionCount++;
            lastOldState = oldS;
            lastNewState = newS;
        });
    }

    std::unique_ptr<RedAlertStateMachine> sm;
    int transitionCount;
    State lastOldState;
    State lastNewState;
};

TEST_F(StateMachineTest, InitialState) {
    EXPECT_TRUE(std::holds_alternative<Initialization>(sm->getState()));
}

TEST_F(StateMachineTest, InitializationToNoAlerts) {
    sm->processEvent(WifiConnected{});
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(sm->getState()));
    EXPECT_EQ(transitionCount, 1);
    EXPECT_TRUE(std::holds_alternative<Initialization>(lastOldState));
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(lastNewState));
}

TEST_F(StateMachineTest, NoAlertsToYellow) {
    sm->processEvent(WifiConnected{});
    sm->processEvent(RemoteAlertEvent{});
    EXPECT_TRUE(std::holds_alternative<YellowAlert>(sm->getState()));
    EXPECT_EQ(transitionCount, 2);
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(lastOldState));
    EXPECT_TRUE(std::holds_alternative<YellowAlert>(lastNewState));
}

TEST_F(StateMachineTest, YellowToNoAlerts) {
    sm->processEvent(WifiConnected{});
    sm->processEvent(RemoteAlertEvent{});
    sm->processEvent(NoAlertsEvent{});
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(sm->getState()));
    EXPECT_EQ(transitionCount, 3);
    EXPECT_TRUE(std::holds_alternative<YellowAlert>(lastOldState));
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(lastNewState));
}

TEST_F(StateMachineTest, YellowToEarlyWarning) {
    sm->processEvent(WifiConnected{});
    sm->processEvent(RemoteAlertEvent{});
    sm->processEvent(EarlyWarningEvent{});
    EXPECT_TRUE(std::holds_alternative<EarlyWarning>(sm->getState()));
    EXPECT_EQ(transitionCount, 3);
    EXPECT_TRUE(std::holds_alternative<YellowAlert>(lastOldState));
    EXPECT_TRUE(std::holds_alternative<EarlyWarning>(lastNewState));
}

TEST_F(StateMachineTest, YellowToRedAlert) {
    sm->processEvent(WifiConnected{});
    sm->processEvent(RemoteAlertEvent{});
    sm->processEvent(RedAlertEvent{});
    EXPECT_TRUE(std::holds_alternative<RedAlert>(sm->getState()));
    EXPECT_EQ(transitionCount, 3);
    EXPECT_TRUE(std::holds_alternative<YellowAlert>(lastOldState));
    EXPECT_TRUE(std::holds_alternative<RedAlert>(lastNewState));
}

TEST_F(StateMachineTest, NoAlertsToEarlyWarning) {
    sm->processEvent(WifiConnected{});
    sm->processEvent(EarlyWarningEvent{});
    EXPECT_TRUE(std::holds_alternative<EarlyWarning>(sm->getState()));
    EXPECT_EQ(transitionCount, 2);
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(lastOldState));
    EXPECT_TRUE(std::holds_alternative<EarlyWarning>(lastNewState));
}

TEST_F(StateMachineTest, EarlyWarningToRedAlert) {
    sm->processEvent(WifiConnected{});
    sm->processEvent(EarlyWarningEvent{});
    sm->processEvent(RedAlertEvent{});
    EXPECT_TRUE(std::holds_alternative<RedAlert>(sm->getState()));
    EXPECT_EQ(transitionCount, 3);
    EXPECT_TRUE(std::holds_alternative<EarlyWarning>(lastOldState));
    EXPECT_TRUE(std::holds_alternative<RedAlert>(lastNewState));
}

TEST_F(StateMachineTest, NoAlertsToRedAlert) {
    sm->processEvent(WifiConnected{});
    sm->processEvent(RedAlertEvent{});
    EXPECT_TRUE(std::holds_alternative<RedAlert>(sm->getState()));
    EXPECT_EQ(transitionCount, 2);
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(lastOldState));
    EXPECT_TRUE(std::holds_alternative<RedAlert>(lastNewState));
}

TEST_F(StateMachineTest, RedAlertToNoAlerts) {
    sm->processEvent(WifiConnected{});
    sm->processEvent(RedAlertEvent{});
    sm->processEvent(EventEndedEvent{});
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(sm->getState()));
    EXPECT_EQ(transitionCount, 3);
    EXPECT_TRUE(std::holds_alternative<RedAlert>(lastOldState));
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(lastNewState));
}

TEST_F(StateMachineTest, EarlyWarningToNoAlerts) {
    sm->processEvent(WifiConnected{});
    sm->processEvent(EarlyWarningEvent{});
    sm->processEvent(EventEndedEvent{});
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(sm->getState()));
    EXPECT_EQ(transitionCount, 3);
    EXPECT_TRUE(std::holds_alternative<EarlyWarning>(lastOldState));
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(lastNewState));
}

TEST_F(StateMachineTest, StayInState) {
    sm->processEvent(WifiConnected{});
    int countBefore = transitionCount;
    sm->processEvent(NoAlertsEvent{});
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(sm->getState()));
    EXPECT_EQ(transitionCount, countBefore); // Index check in processEvent should prevent callback
}

TEST_F(StateMachineTest, InvalidTransition) {
    sm->processEvent(WifiConnected{});
    int countBefore = transitionCount;
    sm->processEvent(WifiConnected{}); // Already connected
    EXPECT_TRUE(std::holds_alternative<NoAlerts>(sm->getState()));
    EXPECT_EQ(transitionCount, countBefore);
}