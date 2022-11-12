#ifndef SANDBOX_ENTITY_H_
#define SANDBOX_ENTITY_H_

#include <vector>
#include "component.h"

namespace sandbox {

class Entity {
public:
    Entity() : parent(nullptr) {}
    virtual ~Entity();

    Component& addComponent(Component* component);
    void deleteComponent(Component* component);
    const std::vector<Component*>& getComponents() const { return components; }

    Entity& addChild(Entity* child);
    void deleteChild(Entity* child);
    const std::vector<Entity*>& getChildren() const { return children; }

    Entity* getParent() { return parent; }

private:
    void removeComponent(Component* component);
    void removeChild(Entity* child);
    void setParent(Entity* parent) { this->parent = parent; }

    std::vector<Component*> components;
    std::vector<Entity*> children;
    Entity* parent;
};

}

#endif