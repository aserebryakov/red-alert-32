#include "RedAlertStateMachine.h"

#include <HardwareSerial.h>
#include <iostream>
#include <ostream>


RedAlertStateMachine::RedAlertStateMachine(TransitionCallback callback)
    : currentState(Initialization{}), onTransition(callback) {}


void RedAlertStateMachine::processEvent(const Event& event) {
    auto visitor = [this](auto&& s, auto&& e) -> State {
        return transition(s, e);
    };

    State nextState = std::visit(visitor, currentState, event);

    if (nextState.index() != currentState.index()) {
        if (onTransition) {
            onTransition(currentState, nextState);
        }
        currentState = nextState;
    }
}


const State& RedAlertStateMachine::getState() const {
    return currentState;
}


State RedAlertStateMachine::transition(Initialization, WifiConnectedEvent) { return NoAlerts{}; }

State RedAlertStateMachine::transition(NoAlerts, NoAlertsEvent) { return NoAlerts{}; }
State RedAlertStateMachine::transition(NoAlerts, DistantAlertEvent) { return YellowAlert{}; }
State RedAlertStateMachine::transition(NoAlerts, EarlyWarningEvent) { return EarlyWarning{}; }
State RedAlertStateMachine::transition(NoAlerts, RedAlertEvent) { return RedAlert{}; }

State RedAlertStateMachine::transition(YellowAlert, EarlyWarningEvent) { return EarlyWarning{}; }
State RedAlertStateMachine::transition(YellowAlert, RedAlertEvent) { return RedAlert{}; }
State RedAlertStateMachine::transition(YellowAlert, NoAlertsEvent) { return NoAlerts{}; }

State RedAlertStateMachine::transition(EarlyWarning, RedAlertEvent) { return RedAlert{}; }
State RedAlertStateMachine::transition(EarlyWarning, EventEndedEvent) { return NoAlerts{}; }

State RedAlertStateMachine::transition(RedAlert, EventEndedEvent) { return NoAlerts{}; }
