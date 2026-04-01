#ifndef RED_ALERT_32_WEBSERVERMANAGER_H
#define RED_ALERT_32_WEBSERVERMANAGER_H

#include <WebServer.h>
#include "ConfigurationManager.h"
#include <functional>

using ConfigurationCallback = std::function<void(const Configuration&, bool)>;

class WebServerManager {
public:
    WebServerManager(int port = 80);
    void begin();
    void loop();
    void setConfigurationCallback(ConfigurationCallback &&callback);

private:
    void handleRootGet();
    void handleRootPost();
    WebServer server;
    ConfigurationCallback callback;
};

#endif //RED_ALERT_32_WEBSERVERMANAGER_H
