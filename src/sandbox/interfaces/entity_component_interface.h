#ifndef SANDBOX_INTERFACES_ENTITY_COMPONENT_INTERFACE_H_
#define SANDBOX_INTERFACES_ENTITY_COMPONENT_INTERFACE_H_

#include <vector>
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/component.h"
#include "sandbox/entity.h"

namespace sandbox {

template <typename BASE>
class ObjectFactory {
public:
    virtual ~ObjectFactory() {}
    virtual BASE* create(const std::string& typeName) = 0;
};

template <typename T, typename BASE>
class TypedObjectFactory : public ObjectFactory<BASE> {
public:
    TypedObjectFactory(const std::string& typeName) : typeName(typeName) {}
    virtual ~TypedObjectFactory() {}
    BASE* create(const std::string& typeName) { 
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

template <typename BASE>
class CompositeObjectFactory {
public:
    virtual ~CompositeObjectFactory() {}

    template <typename T>
    void addType(const std::string& typeName) { addFactory(new TypedObjectFactory<T, BASE>(typeName)); }
    void addFactory(ObjectFactory<BASE>* factory) {
        factories.push_back(factory);
    }
    virtual BASE* create(const std::string& typeName) {
        for (int i = 0; i < factories.size(); i++) {
            BASE* item = factories[i]->create(typeName);
            if (item) {
                return item;
            }
        }

        return NULL;
    }

private:
    std::vector< ObjectFactory<BASE>* > factories;
};

typedef ObjectFactory<Component> ComponentFactory;
typedef ObjectFactory<Task> TaskFactory;

class EntityComponentInterface : public PluginInterface {
public:
    virtual ~EntityComponentInterface() {}

    Component* createComponent(const std::string& typeName) {
        return componentFactory.create(typeName);
    }

    Task* createTask(const std::string& typeName) {
        return taskFactory.create(typeName);
    }

    CompositeObjectFactory<Component>& getComponentFactory() { return componentFactory; }
    CompositeObjectFactory<Task>& getTaskFactory() { return taskFactory; }

private:
    CompositeObjectFactory<Component> componentFactory;
    CompositeObjectFactory<Task> taskFactory;
};

}

#endif