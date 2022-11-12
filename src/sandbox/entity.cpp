#include "entity.h"
#include <algorithm>

namespace sandbox {

Entity::~Entity() {
    for (int i = 0; i < components.size(); i++) {
        delete components[i];
    }

    for (int i = 0; i < children.size(); i++) {
        delete children[i];
    }
}

Component& Entity::addComponent(Component* component) {
    if (component->getEntity() != NULL) {
        component->getEntity()->removeComponent(component);
    }
    component->setEntity(*this);
    components.push_back(component);
    return *component;
}

void Entity::deleteComponent(Component* component) {
    removeComponent(component);
    delete component;
}

void Entity::removeComponent(Component* component) {
    std::vector<Component*>::iterator position = std::find(components.begin(), components.end(), component);
    if (position != components.end()) {
        components.erase(position);
    }
}

Entity& Entity::addChild(Entity* child) {
    if (child->getParent() != NULL) {
        child->getParent()->removeChild(child);
    }
    child->setParent(this);
    children.push_back(child);
    return *child;
}

void Entity::deleteChild(Entity* child) {
    removeChild(child);
    delete child;
}

void Entity::removeChild(Entity* child) {
    std::vector<Entity*>::iterator position = std::find(children.begin(), children.end(), child);
    if (position != children.end()) {
        children.erase(position);
    }
}

}