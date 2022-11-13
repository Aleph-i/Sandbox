#ifndef SANDBOX_INTERFACES_ENTITY_COMPONENT_INTERFACE_H_
#define SANDBOX_INTERFACES_ENTITY_COMPONENT_INTERFACE_H_

#include <vector>
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/component.h"
#include "sandbox/entity.h"
#include "sandbox/factory/object_factory.h"

namespace sandbox {

class EntityComponentInterface : public PluginInterface {
public:
    virtual ~EntityComponentInterface() {}

    CompositeObjectFactory<Component>& components() { return componentFactory; }
    CompositeObjectFactory<Task>& tasks() { return taskFactory; }

private:
    CompositeObjectFactory<Component> componentFactory;
    CompositeObjectFactory<Task> taskFactory;
};

}

#endif