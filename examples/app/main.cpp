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


class UpdateCommand : public sandbox::CallbackComponent {
public:
    UpdateCommand(std::mutex& mutex) : mutex(mutex) {}
    void run(sandbox::Object& data, sandbox::Object& returnValue) {
        std::unique_lock<std::mutex> lock(mutex);
        std::cout << "Something: " << data["id"].get<double>() << std::endl;
    }

private:
    std::mutex& mutex;
};


class CursorPosition : public sandbox::CallbackComponent {
public:
    void run(sandbox::Object& data, sandbox::Object& returnValue) {
        std::cout << "Something: " << data["x"].get<double>() << " " << data["y"].get<double>() << std::endl;
    }
};

class MouseButton : public sandbox::CallbackComponent {
public:
    void run(sandbox::Object& data, sandbox::Object& returnValue) {
        std::cout << "Something: " << data["button"].get<int>() << " " << data["action"].get<int>() << " " << data["mods"].get<int>() << std::endl;
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

    std::mutex updateMutex;

    Entity root("Root");
        Entity& display = root.addChild(new Entity("Display"));
            Component& window = display.addComponent(ec->components().create("GLFWWindow"));
                window["width"].set<int>(700);
            Entity& triangle = display.addChild(new Entity("Triangle"));
                triangle.addComponent(ec->components().create("OpenGLTest"));
        Entity& display2 = root.addChild(new Entity("Display"));
            Component& window2 = display2.addComponent(ec->components().create("GLFWWindow"));
                window2["width"].set<int>(700);
            Entity& triangle2 = display2.addChild(new Entity("Triangle"));
                Component& t2 = triangle2.addComponent(ec->components().create("OpenGLTest"));
                t2["dir"].set<bool>(false);
        Entity& geometry = root.addChild(new Entity("Geometry"));
            Entity& object = geometry.addChild(new Entity("Object"));
                Component& mesh = object.addComponent(ec->components().create("AssimpMesh"));
                mesh["filePath"].set<std::string>("../examples/app/data/models/monkey-head.obj");
        Entity& graphics = root.addChild(new Entity("Graphics"));
            //graphics.addComponent(new renderer());
            //graphics.addComponent(proxy(display));
    
    root.update();


    Task& print = *ec->tasks().create("PrintTask");
    root.runTask(print);

    Task& swapBuffers = *ec->tasks().create("GLFWSwapBuffers");
    Task& pollEvents = *ec->tasks().create("GLFWPollEvents");
    Task& makeCurrent = *ec->tasks().create("GLFWMakeCurrent");
    Task& init = *ec->tasks().create("OpenGLInit");
    Task& run = *ec->tasks().create("OpenGLRun");
    Task& render = *ec->tasks().create("GLFWRender");

    root.runTask(makeCurrent);
    /*root.runTask(init);
    root.runTask(run);
    root.runTask(swapBuffers);
    root.runTask(pollEvents);*/

    while (true) {
        //usleep(100000);
        /*
        root.runTask(makeCurrent);
        root.runTask(run);
        root.runTask(swapBuffers);
        */
        display.runTask(render);
        display2.runTask(render);
        root.runTask(pollEvents);
    }

    return 0;
}