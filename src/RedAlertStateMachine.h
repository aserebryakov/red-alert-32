#ifndef RED_ALERT_STATE_MACHINE_H
#define RED_ALERT_STATE_MACHINE_H

#include <variant>
#include <functional>

struct Initialization {};
struct NoAlerts {};
struct YellowAlert {};
struct EarlyWarning {};
struct RedAlert {};

using State = std::variant<Initialization, NoAlerts, YellowAlert, EarlyWarning, RedAlert>;

struct WifiConnectedEvent {};
struct NoAlertsEvent {};
struct DistantAlertEvent {};
struct EarlyWarningEvent {};
struct RedAlertEvent {};
struct EventEndedEvent {};
struct ErrorEvent {};

using Event = std::variant<WifiConnectedEvent, NoAlertsEvent, DistantAlertEvent, EarlyWarningEvent, RedAlertEvent, EventEndedEvent, ErrorEvent>;

class RedAlertStateMachine {
public:
    using TransitionCallback = std::function<void(const State&, const State&)>;

    // Single transition callback is not ideal but we can live with it.
    RedAlertStateMachine(TransitionCallback callback = nullptr);

    void processEvent(const Event& event);

    const State& getState() const;

private:
    State currentState;
    TransitionCallback onTransition;

    template<typename S, typename E>
    State transition(S, E) {
        return currentState;
    }

    State transition(Initialization, WifiConnectedEvent);
    
    State transition(NoAlerts, NoAlertsEvent);
    State transition(NoAlerts, DistantAlertEvent);
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
