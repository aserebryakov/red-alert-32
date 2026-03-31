#ifndef RED_ALERT_32_WEBSERVERMANAGER_H
#define RED_ALERT_32_WEBSERVERMANAGER_H

#include <WebServer.h>

class WebServerManager {
public:
    WebServerManager(int port = 80);
    void begin();
    void loop();

private:
    void handleRootGet();
    void handleRootPost();
    WebServer server;
};

#endif //RED_ALERT_32_WEBSERVERMANAGER_H
