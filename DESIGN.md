# Red Alert 32

ESP32 based red alert monitor.

## Idea

Monitor server of Home Front for alerts and indicate different states based on received messages.

## States

* No alerts (green LED)
* Alerts in other regions (yellow LED)
* Early notification (red LED)
* Red alert (red LED is blinking)
* Error (Yellow LED is blinking)

Special behavior is intended for RED alert state: RED alert state is removed only after notification
of finished event is received.

## Generic Flow

```mermaid
sequenceDiagram
    participant Device
    participant HomeFrontServer
    
    loop once a second
        Device ->> HomeFrontServer: Get Alerts.json
        HomeFrontServer ->> Device: Alerts.json
        note over Device: extract data<br>and update state
    end
```

## State Machine

```mermaid
stateDiagram
    [*] --> Initialization
    Initialization --> NoAlerts: Wifi Connected
    NoAlerts --> NoAlerts : Alerts.json is empty
    NoAlerts --> YellowAlert : Alerts.json is not empty
    YellowAlert --> EarlyNotification : Early notification
    YellowAlert --> RedAlert : Red alert
    NoAlerts --> EarlyNotification : Early notification
    EarlyNotification --> RedAlert : Red alert
    NoAlerts --> RedAlert : Red alert
    RedAlert --> NoAlerts: Event finished
    EarlyNotification --> NoAlerts: Event finished
    YellowAlert --> NoAlerts : Alerts.json is empty
```
