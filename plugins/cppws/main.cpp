#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"
#include "WebServer.h"
#include <map>
#include <mutex>
#include "sandbox/object/object.h"
#include "pico_json_object.h"


class WebCommand {
public:
    virtual ~WebCommand() {}

    virtual void run(const std::string& cmd, sandbox::Object& data, sandbox::Object& returnValue) = 0;
};

/*class WebUpdateCommand : public WebCommand {
public:
    void run(const std::string& cmd, sandbox::Object& data, sandbox::Object& returnValue) {
        std::cout << cmd << " was run. " << data["simSpeed"].get<double>() << "  " << data["id"].get<double>() << std::endl;
        //{"command":"update","delta":0,"id":142,"simSpeed":1}
    }
};*/

struct WebAppServerState {
    std::map<std::string, WebCommand*> commands;
};

class WebAppService : public JSONSession {
public:
    WebAppService(WebAppServerState& state) : state(state) {
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
        std::map<std::string, WebCommand*>::iterator it = state.commands.find(cmd);
        if (it != state.commands.end()) {
            PicoJsonObject d(data);
            PicoJsonObject r(returnValue);
            it->second->run(cmd, d, r);
        }
        else {
            std::cout << "Unknown command: " << cmd << " - " << picojson::value(data).serialize() << std::endl;
        }
    }

private:
    WebAppServerState& state;
};

class WebAppServer : public WebServerBase {
public:
	WebAppServer(int port = 8081, const std::string& webDir = ".") : WebServerBase(port, webDir) {
        //state.commands["update"] = new WebUpdateCommand();
    }

    void addWebCommand(const std::string& cmd, WebCommand* command) {
        state.commands[cmd] = command;
    }

protected:
	Session* createSession() { return new WebAppService(state); }

private:
    WebAppServerState state;
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

    void addWebCommand(const std::string& cmd, WebCommand* command) {
        webServer->addWebCommand(cmd, command);
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

class WebCommandComponent : public sandbox::Component, public WebCommand {
public:
    WebCommandComponent() : loaded(false) {
        addType<WebCommandComponent>();
        addType<WebCommand>(static_cast<WebCommand*>(this));
        addAttribute(new sandbox::TypedAttributeRef<std::string>("command", command));
    }

    void update() {
        if (!loaded) {
            CppWebServer* ws = this->getEntity()->getComponentFromAbove<CppWebServer>();
            if (ws) {
                ws->addWebCommand(command, this);
                callbacks = getEntity()->getComponents<sandbox::CallbackComponent>();
            }

            loaded = true;
        }
    }

    void run(const std::string& cmd, sandbox::Object& data, sandbox::Object& returnValue) {
        for (int i = 0; i < callbacks.size(); i++) {
            callbacks[i]->run(data, returnValue);
        }
    }

private:
    bool loaded;
    std::string command;
    std::vector<sandbox::CallbackComponent*> callbacks;
};

/*class WebUpdateCommand : public WebCommandComponent {
public:
    WebUpdateCommand() : WebCommandComponent() {
        addType<WebUpdateCommand>();
        //createCallback("command");
    }

    void run(const std::string& cmd, sandbox::Object& data, sandbox::Object& returnValue) {
        std::cout << cmd << " was run. " << data["simSpeed"].get<double>() << "  " << data["id"].get<double>() << std::endl;
        //{"command":"update","delta":0,"id":142,"simSpeed":1}
        //runCallback("command", data, returnValue);
        
    }
};*/

extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<CppWebServer>("CppWebServer");
            ec->components().addType<WebCommandComponent>("WebCommandComponent");
        }
	}
}