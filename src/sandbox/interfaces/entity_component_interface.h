#ifndef SANDBOX_INTERFACES_ENTITY_COMPONENT_INTERFACE_H_
#define SANDBOX_INTERFACES_ENTITY_COMPONENT_INTERFACE_H_

#include <vector>
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/component.h"

namespace sandbox {

class ComponentFactory {
public:
    virtual ~ComponentFactory() {}
    virtual Component* create(const std::string& typeName) = 0;
};

template <typename T>
class TypedComponentFactory : public ComponentFactory {
public:
    TypedComponentFactory(const std::string& typeName) : typeName(typeName) {}
    virtual ~TypedComponentFactory() {}
    Component* create(const std::string& typeName) { 
        if (typeName == this->typeName) {
            return new T(); 
        }
        else {
            return NULL;
        } 
    }
private:
    std::string typeName;
};

class EntityComponentInterface : public PluginInterface {
public:
    virtual ~EntityComponentInterface() {}

    template <typename T>
    void addType(const std::string& typeName) { addComponentFactory(new TypedComponentFactory<T>(typeName)); }

    void addComponentFactory(ComponentFactory* factory) {
        componentFactories.push_back(factory);
    }

    Component* createComponent(const std::string& typeName) {
        for (int i = 0; i < componentFactories.size(); i++) {
            Component* comp = componentFactories[i]->create(typeName);
            if (comp) {
                return comp;
            }
        }

        return NULL;
    }

private:
    std::vector<ComponentFactory*> componentFactories;
};

}

#endif