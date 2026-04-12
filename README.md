# Red Alert 32

> Weird times make weird IoT projects.

This device is a simple IoT project that monitors Home Front alerts.

**IMPORTANT**: This implementation is mostly fast and dirty and not intended to be used as a main way to be notified of
alerts. Please use the official means.

## Features

- Real-time monitoring of alert messages.
- Web-based configuration interface.
- Visual LED alerts for different states.

## Hardware Configuration

The default LED pins used are:
- **Green LED**: Pin 19
- **Yellow LED**: Pin 18
- **Red LED**: Pin 23

## Initial Setup

If the device has no stored WiFi configuration, it will enter **Access Point (AP) mode**:

1. Connect to the WiFi network named **`RED_ALERT_32`**.
2. Open a web browser and navigate to `http://red-alert-32.local`.
3. Enter your WiFi SSID, Password, and your City Name.
4. Save the configuration. The device will restart and attempt to connect to your WiFi.

## LED Indicators Meaning

The device communicates its status through the LEDs:

- **Red and Green Blinking**: The device is attempting to connect to the WiFi network.
- **Green (Solid)**: Connected to WiFi, no active alerts for the configured city.
- **Yellow (Solid)**: Active alert in some other location.
- **Red (Solid)**: Active early warning in your city.
- **Red (Blinking)**: Active red alert.

## Known Issues

1. No indication if it is in initial setup mode.
2. No error handling on receiving data.
3. Blinking is inconsistent.