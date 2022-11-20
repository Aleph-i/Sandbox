#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"
#include "WebServer.h"

class WebAppService : public JSONSession {
public:
    WebAppService() {
    }

    void receiveJSON(picojson::value& val) {
        picojson::object data = val.get<picojson::object>();
        std::string cmd = data["command"].get<std::string>();
        picojson::object returnValue;
        returnValue["id"] = data["id"];
        ReceiveCommand(cmd, data, returnValue);
        picojson::value retVal(returnValue);
        sendJSON(retVal);
    }

    void ReceiveCommand(const std::string& cmd, picojson::object& data, picojson::object& returnValue) {
        std::cout << "Unknown command: " << cmd << " - " << picojson::value(data).serialize() << std::endl;
    }

private:
};

class WebAppServer : public WebServerBase {
public:
	WebAppServer(int port = 8081, const std::string& webDir = ".") : WebServerBase(port, webDir) {}

protected:
	Session* createSession() { return new WebAppService(); }
private:
};


class CppWebServer : public sandbox::Component {
public:
    CppWebServer() : webServer(NULL) {
        addType<CppWebServer>();
        addAttribute(new sandbox::TypedAttributeRef<int>("port", port, 8081));
        addAttribute(new sandbox::TypedAttributeRef<std::string>("webDir", webDir, "."));
    }

    ~CppWebServer() {
        delete webServer;
    }

    void update() {
        if (!webServer) {
            webServer = new WebAppServer(port, webDir);
        }

        webServer->service();
    }

private:
    WebAppServer* webServer;
    int port;
    std::string webDir;
};


extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<CppWebServer>("CppWebServer");
        }
	}
}