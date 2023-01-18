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
    update();
    return *component;
}

void Entity::deleteComponent(Component* component) {
    removeComponent(component);
    delete component;
    update();
}

void Entity::removeComponent(Component* component) {
    std::vector<Component*>::iterator position = std::find(components.begin(), components.end(), component);
    if (position != components.end()) {
        components.erase(position);
    }
    update();
}

Entity& Entity::addChild(Entity* child) {
    if (child->getParent() != NULL) {
        child->getParent()->removeChild(child);
    }
    child->setParent(this);
    children.push_back(child);
    update();
    return *child;
}

void Entity::deleteChild(Entity* child) {
    removeChild(child);
    delete child;
    update();
}

void Entity::removeChild(Entity* child) {
    std::vector<Entity*>::iterator position = std::find(children.begin(), children.end(), child);
    if (position != children.end()) {
        children.erase(position);
    }
    update();
}

void Entity::runTask(Task& task) {
    TaskContext* context = task.createContext();
    runTask(task, context);
    delete context;
}

void Entity::runTask(Task& task, TaskContext* context) {
    task.run(*this, context);
}

void Entity::update() {
    const std::vector<Component*>& components = getComponents();
    for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
        (*it)->updateComponent();
    }

    const std::vector<Entity*>& entities = getChildren();
    for (std::vector<Entity*>::const_iterator it = entities.begin(); it != entities.end(); it++) {
        (*it)->update();
    }
}

Attribute& Entity::operator[](const std::string& name) const {
    const std::vector<Component*>& components = getComponents();
    for (std::vector<Component*>::const_iterator it= components.begin(); it != components.end(); it++) {
        Attribute& att = (*it)->operator[](name);
        if (&att != &NullAttribute::instance()) {
            return att;
        }
    }


    return NullAttribute::instance();
}


}