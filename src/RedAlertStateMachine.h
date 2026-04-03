#ifndef RED_ALERT_STATE_MACHINE_H
#define RED_ALERT_STATE_MACHINE_H

#include <variant>
#include <functional>
#include <iostream>

// States
struct Initialization {};
struct NoAlerts {};
struct YellowAlert {};
struct EarlyWarning {};
struct RedAlert {};

using State = std::variant<Initialization, NoAlerts, YellowAlert, EarlyWarning, RedAlert>;

// Events
struct WifiConnected {};
struct NoAlertsEvent {};
struct RemoteAlertEvent {};
struct EarlyWarningEvent {};
struct RedAlertEvent {};
struct EventEndedEvent {};
struct ErrorEvent {};

using Event = std::variant<WifiConnected, NoAlertsEvent, RemoteAlertEvent, EarlyWarningEvent, RedAlertEvent, EventEndedEvent, ErrorEvent>;

class RedAlertStateMachine {
public:
    using TransitionCallback = std::function<void(const State&, const State&)>;

    RedAlertStateMachine(TransitionCallback callback = nullptr);

    void processEvent(const Event& event);

    const State& getState() const;

private:
    State currentState;
    TransitionCallback onTransition;

    // Default: stay in current state
    template<typename S, typename E>
    State transition(S, E) {
        return currentState;
    }

    // Specific transitions
    State transition(Initialization, WifiConnected);
    
    State transition(NoAlerts, NoAlertsEvent);
    State transition(NoAlerts, RemoteAlertEvent);
    State transition(NoAlerts, EarlyWarningEvent);
    State transition(NoAlerts, RedAlertEvent);

    State transition(YellowAlert, EarlyWarningEvent);
    State transition(YellowAlert, RedAlertEvent);
    State transition(YellowAlert, NoAlertsEvent);

    State transition(EarlyWarning, RedAlertEvent);
    State transition(EarlyWarning, EventEndedEvent);

    State transition(RedAlert, EventEndedEvent);
};

#endif // RED_ALERT_STATE_MACHINE_H
