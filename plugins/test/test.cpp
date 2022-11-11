#include "sandbox/test.h"

#include <iostream>
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/interfaces/entity_component_interface.h"

class TestComponent : public sandbox::Component {
public:
};

extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;
		std::cout << "Plugin Loaded." << std::endl;
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->addType<TestComponent>("TestComponent");
        }
	}
}