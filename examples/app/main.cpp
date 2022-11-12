#include <iostream>

#include "sandbox/plugin/plugin_manager.h"
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox/entity.h"

class TestPluginInterface : public sandbox::PluginInterface {
public:
};

void printEntity(sandbox::Entity& entity, std::string indent = "") {
    using namespace sandbox;
    std::cout << indent << &entity << "(" << entity.getParent() << ")" << std::endl;
    const std::vector<Component*>& components = entity.getComponents();
    for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
        if (*it != NULL) {
            std::cout << indent << "- " << *it << "(" << (*it)->getEntity() << ")" << std::endl;
        }
    }

    const std::vector<Entity*>& entities = entity.getChildren();
    for (std::vector<Entity*>::const_iterator it = entities.begin(); it != entities.end(); it++) {
        if (*it != NULL) {
            printEntity(**it, indent + "\t");
        }
    }
}

int main(int argc, char *argv[]) {
    std::cout << "This is a test." << std::endl;

    using namespace sandbox;

    PluginManager pm;
    pm.addPluginInterface(new TestPluginInterface());
    EntityComponentInterface* ec = new EntityComponentInterface();
    pm.addPluginInterface(ec);
    pm.loadPlugin("lib/libtest.so");

    Entity root;
    root.addComponent(ec->createComponent("TestComponent"));
    root.addComponent(ec->createComponent("TestComponent"));
    Entity& next = root.addChild(new Entity());
    next.addComponent(ec->createComponent("TestComponent"));
    Entity& next2 = root.addChild(new Entity());
    printEntity(root);
    next.addChild(&next2);

    printEntity(root);


    return 0;
}