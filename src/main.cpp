#include <DIYables_ESP32_WebServer.h>
#include <Arduino.h>
#include "RedAlertManager.h"
#include <EEPROM.h>

DIYables_ESP32_WebServer server;
RedAlertManager red_alert_manager;

const char HOME_PAGE[] = R"(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Submit Form</title>
<style>
    body {
        font-family: Arial, sans-serif;
        padding: 40px;
    }

form {
    max-width: 400px;
}

input {
    width: 100%;
    padding: 8px;
    margin: 8px 0;
    box-sizing: border-box;
}

button {
    padding: 10px 15px;
    cursor: pointer;
}
</style>
</head>
<body>

<h2>Send Data to Server</h2>

<form method="POST">
    <label>SSID</label>
    <input type="text" id="ssid" name="SSID">

    <label>Password</label>
    <input type="text" id="password" name="Password">

    <label>City</label>
    <input type="text" id="city" name="City">

    <label>Reset</label>
    <input type="checkbox" id="reset" name="Reset">

    <button type="submit">Submit</button>
</form>

</body>
</html>)";

void handleHome(WiFiClient& client, const String& method, const String& request, const QueryParams& params, const String& jsonData) {
    Serial.println(method);
    Serial.println(jsonData);
    Serial.println(EEPROM.writeString(0, jsonData));
    Serial.println(EEPROM.read(0));
    EEPROM.commit();
    server.sendResponse(client, HOME_PAGE);
}

void setup() {
    Serial.begin(9600);
    EEPROM.begin(1024);
    red_alert_manager.begin();
    server.addRoute("/", handleHome);
    server.begin();

    Serial.println("Read from EEPROM");
    const auto read = EEPROM.readString(0);
    Serial.println(read);
    Serial.println(EEPROM.read(0));

    Serial.println("\n=== Web Server Ready! ===");
    Serial.print("Visit: http://");
    Serial.println(WiFi.localIP());
}

void loop() {
    server.handleClient();
    Serial.println("Read from EEPROM");
    const auto read = EEPROM.readString(0);
    Serial.println(read);
    Serial.println(EEPROM.read(0));
    red_alert_manager.loop();
}
