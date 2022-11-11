#include <iostream>

#include "sandbox/plugin/plugin_manager.h"
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/interfaces/entity_component_interface.h"

class TestPluginInterface : public sandbox::PluginInterface {
public:
};

int main(int argc, char *argv[]) {
    std::cout << "This is a test." << std::endl;

    using namespace sandbox;

    PluginManager pm;
    pm.addPluginInterface(new TestPluginInterface());
    pm.addPluginInterface(new TestPluginInterface());
    EntityComponentInterface* ec = new EntityComponentInterface();
    pm.addPluginInterface(ec);
    pm.loadPlugin("lib/libtest.so");
    std::cout << ec->createComponent("TestComponent") << std::endl;

    return 0;
}