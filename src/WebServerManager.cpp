#include "WebServerManager.h"
#include <Arduino.h>
#include <WiFi.h>

const char HOME_PAGE[] = R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Configuration</title>
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

<h2>Configuration</h2>

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

const char SUBMITTED[] = R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Submitted</title>
</head>
<body>

<h2>Submitted</h2>

</body>
</html>)";

WebServerManager::WebServerManager(int port) : server(port) {}

void WebServerManager::begin() {
    server.on("/", HTTP_GET, [this]() {
        this->handleRootGet();
    });

    server.on("/", HTTP_POST, [this]() {
        this->handleRootPost();
    });

    server.begin();

    Serial.println("\n=== Web Server Ready! ===");
    Serial.print("Visit: http://");
    Serial.println(WiFi.localIP());
}

void WebServerManager::setConfigurationCallback(ConfigurationCallback &&cb) {
    callback = std::move(cb);
}

void WebServerManager::loop() {
    server.handleClient();
}

void WebServerManager::handleRootGet() {
    Serial.println("GET /");
    server.send(200, "text/html", HOME_PAGE);
}

void WebServerManager::handleRootPost() {
    Serial.println("POST /");
    Configuration config;
    bool resetFlag = false;

    if (server.hasArg("SSID")) {
        config.ssid = server.arg("SSID");
    }
    if (server.hasArg("Password")) {
        config.password = server.arg("Password");
    }
    if (server.hasArg("City")) {
        config.cityName = server.arg("City");
    }
    if (server.hasArg("Reset")) {
        resetFlag = (server.arg("Reset") == "on");
    }

    server.send(200, "text/html", SUBMITTED);

    if (callback) {
        callback(config, resetFlag);
    }
}
