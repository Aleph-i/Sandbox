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
    root.addComponent(ec->createComponent("TestComponent"));
    Component* console = ec->createComponent("ConsoleDisplay");
    root.addComponent(console);
    Entity& next = root.addChild(new Entity("Next"));
    next.addComponent(ec->createComponent("TestComponent"));
    Entity& next2 = root.addChild(new Entity("Next2"));
    //printEntity(root);
    next.addChild(&next2);

    //printEntity(root);
    Task& print = *ec->createTask("PrintTask");
    Task& render = *ec->createTask("RenderTask");
    root.runTask(print);
    next.runTask(print);

    console->asType<Renderable>()->Render();
    root.runTask(render);

    return 0;
}