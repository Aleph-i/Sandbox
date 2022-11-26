#include <iostream>

#include "sandbox/plugin/plugin_manager.h"
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox/entity.h"
#include <unistd.h>
#include "sandbox_threading/async_update.h"

class PrintTest : public sandbox::Component {
public:
    PrintTest() {
        addType<PrintTest>();
    }

    void update() {
        std::cout << "PrintTest" << std::endl;
    }
};


class UpdateCommand : public sandbox::ComponentCallback {
public:
    void run(sandbox::Component& component, sandbox::Object& data, sandbox::Object& returnValue) {
        std::cout << "Something: " << data["id"].get<double>() << std::endl;
    }
};


int main(int argc, char *argv[]) {
    std::cout << "This is a test." << std::endl;

    using namespace sandbox;

    PluginManager pm;
    EntityComponentInterface* ec = new EntityComponentInterface();
    pm.addPluginInterface(ec);
    pm.loadPlugin("lib/libtest.so");
    pm.loadPlugin("lib/libglfw_sandbox.so");
    pm.loadPlugin("lib/libopengl_sandbox.so");
    pm.loadPlugin("lib/libstb_sandbox.so");
    pm.loadPlugin("lib/libassimp_sandbox.so");
    pm.loadPlugin("lib/libcppws_sandbox.so");
    pm.loadPlugin("lib/libpicojson_sandbox.so");
    ec->components().addType<sandbox::AsyncUpdate>("AsyncUpdate");

    Entity root("Root");
        Entity& display = root.addChild(new Entity("Display"));
            Component& window = display.addComponent(ec->components().create("GLFWWindow"));
                window["width"].set<int>(700);
            Entity& triangle = display.addChild(new Entity("Triangle"));
                triangle.addComponent(ec->components().create("OpenGLTest"));
        Entity& geometry = root.addChild(new Entity("Geometry"));
            Entity& object = geometry.addChild(new Entity("Object"));
                Component& mesh = object.addComponent(ec->components().create("AssimpMesh"));
                mesh["filePath"].set<std::string>("../examples/app/data/models/monkey-head.obj");
        Entity& images = root.addChild(new Entity("Images"));
            Component& image = images.addComponent(ec->components().create("STBImageRGBA"));
            image["filePath"].set<std::string>("../examples/app/data/images/img_test.png");
        Entity& config = root.addChild(new Entity("Config"));
            Component& jsonFile = config.addComponent(ec->components().create("PicoJsonFile"));
            jsonFile["filePath"].set<std::string>("../examples/app/data/config/test.json");
            Component& parsedTree = config.addComponent(ec->components().create("PicoJsonParsedTree"));
            parsedTree["entity"].set<Entity*>(&display);
        Entity& webServer = root.addChild(new Entity("WebServer"));
            Entity& webServer1 = webServer.addChild(new Entity("WebServer1"));
                Component& ws = webServer1.addComponent(ec->components().create("CppWebServer"));
                ws["port"].set<int>(8081);
                ws["webDir"].set<std::string>("../examples/app/data/web");
                Component& updateCmd = webServer1.addComponent(ec->components().create("WebUpdateCommand"));
                updateCmd["command"].set<std::string>("update");
                updateCmd.addCallback("command", new UpdateCommand());
            Entity& webServer2 = webServer.addChild(new Entity("WebServer2"));
                Component ws2 = webServer2.addComponent(ec->components().create("CppWebServer"));
                ws2["port"].set<int>(8082);
                ws2["webDir"].set<std::string>("../examples/app/data/web");
            Entity& test = webServer.addChild(new Entity("Test"));
                //test.addComponent(new PrintTest());
        Entity& async = root.addChild(new Entity("Async"));
            Component& wsUpdate = async.addComponent(ec->components().create("AsyncUpdate"));
            wsUpdate["entity"].set<Entity*>(&webServer1);
            Component& wsUpdate2 = async.addComponent(ec->components().create("AsyncUpdate"));
            wsUpdate2["entity"].set<Entity*>(&webServer2);
            Component& wsUpdate3 = async.addComponent(ec->components().create("AsyncUpdate"));
            wsUpdate3["entity"].set<Entity*>(&test);
    
    root.update();

    root.deleteChild(&config);

    Task& print = *ec->tasks().create("PrintTask");
    root.runTask(print);

    //Task& initContext = *ec->tasks().create("GLFWInitContext");
    //root.runTask(initContext);

    Task& swapBuffers = *ec->tasks().create("GLFWSwapBuffers");
    Task& pollEvents = *ec->tasks().create("GLFWPollEvents");
    Task& makeCurrent = *ec->tasks().create("GLFWMakeCurrent");
    Task& init = *ec->tasks().create("OpenGLInit");
    Task& run = *ec->tasks().create("OpenGLRun");

    root.runTask(makeCurrent);
    root.runTask(init);
    root.runTask(run);
    root.runTask(swapBuffers);
    root.runTask(pollEvents);

    //std::mutex* updateMutex = wsUpdate3["updateMutex"].get<std::mutex*>();
    std::mutex updateMutex;

    while (true) {
        usleep(100000);
        std::unique_lock<std::mutex> lock(updateMutex);
        std::cout << "abc" << std::endl;
        lock.unlock();

        root.runTask(makeCurrent);
        root.runTask(run);
        root.runTask(swapBuffers);
        root.runTask(pollEvents);
        //async.update();

        //std::cout << window["width"].get<int>() << std::endl;
        //std::cout << "step" << std::endl;
    }

    return 0;
}