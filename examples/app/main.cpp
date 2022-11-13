#include <iostream>

#include "sandbox/plugin/plugin_manager.h"
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox/entity.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    std::cout << "This is a test." << std::endl;

    using namespace sandbox;

    PluginManager pm;
    EntityComponentInterface* ec = new EntityComponentInterface();
    pm.addPluginInterface(ec);
    pm.loadPlugin("lib/libtest.so");
    pm.loadPlugin("lib/libglfw_sandbox.so");

    Entity root("Root");
    Entity& display = root.addChild(new Entity("Display"));
    display.addComponent(ec->components().create("GLFWWindow"));
    Component& window = display.addComponent(ec->components().create("GLFWWindow"));
    window["width"].set<int>(700);
    window["title"].set<std::string>("Something else");

    Task& print = *ec->tasks().create("PrintTask");
    root.runTask(print);

    Task& initContext = *ec->tasks().create("GLFWInitContext");
    root.runTask(initContext);
    Task& loop = *ec->tasks().create("GLFWLoop");

    while (true) {
        usleep(100000);
        root.runTask(loop);
        std::cout << window["width"].get<int>() << std::endl;
        //std::cout << "step" << std::endl;
    }

    return 0;
}