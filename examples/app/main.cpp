#include <iostream>

#include "sandbox/plugin/plugin_manager.h"
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox/entity.h"

class TestPluginInterface : public sandbox::PluginInterface {
public:
};

int main(int argc, char *argv[]) {
    std::cout << "This is a test." << std::endl;

    using namespace sandbox;

    PluginManager pm;
    pm.addPluginInterface(new TestPluginInterface());
    EntityComponentInterface* ec = new EntityComponentInterface();
    pm.addPluginInterface(ec);
    pm.loadPlugin("lib/libtest.so");

    Entity root("Root");
    root.addComponent(ec->components().create("TestComponent"));
    Component* console = ec->components().create("ConsoleDisplay");
    root.addComponent(console);
    Entity& next = root.addChild(new Entity("Next"));
    next.addComponent(ec->components().create("TestComponent"));
    Entity& next2 = root.addChild(new Entity("Next2"));
    //printEntity(root);
    next.addChild(&next2);

    //printEntity(root);
    Task& print = *ec->tasks().create("PrintTask");
    //Task& render = *ec->tasks().create("RenderTask");
    root.runTask(print);
    next.runTask(print);

    return 0;
}