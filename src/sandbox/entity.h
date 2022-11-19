#ifndef SANDBOX_ENTITY_H_
#define SANDBOX_ENTITY_H_

#include <vector>
#include <string>
#include "component.h"

namespace sandbox {

class TaskContext {
public:
    virtual ~TaskContext() {}
    virtual void push() {}
    virtual void pop() {}
};

class Task {
public:
    virtual ~Task() {}
    virtual void run(Entity& entity, TaskContext* context) = 0;
    virtual TaskContext* createContext() { return NULL; }
};

class RecursiveTask : public Task {
public:
    virtual ~RecursiveTask() {}
    virtual void run(Entity& entity, TaskContext* context);
    virtual void runEntity(Entity& entity, TaskContext* context) = 0;
};

class Entity {
public:
    Entity(const std::string& name) : parent(nullptr), name(name) {}
    virtual ~Entity();

    Component& addComponent(Component* component);
    void deleteComponent(Component* component);
    const std::vector<Component*>& getComponents() const { return components; }

    Entity& addChild(Entity* child);
    void deleteChild(Entity* child);
    const std::vector<Entity*>& getChildren() const { return children; }

    void runTask(Task& task);
    void runTask(Task& task, TaskContext* context);

    Entity* getParent() const { return parent; }
    const std::string& getName() const { return name; }

    void update();

private:
    void removeComponent(Component* component);
    void removeChild(Entity* child);
    void setParent(Entity* parent) { this->parent = parent; }

    std::vector<Component*> components;
    std::vector<Entity*> children;
    Entity* parent;
    std::string name;
};

template <typename T>
class TypedRecursiveTask : public sandbox::RecursiveTask {
public:
    void runEntity(sandbox::Entity& entity, sandbox::TaskContext* context) {
        using namespace sandbox;
        const std::vector<Component*>& components = entity.getComponents();
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            T* component = (*it)->asType<T>();
            if (component) {
                runComponent(*component, context);
            }
        }
    }

    virtual void runComponent(T& component, sandbox::TaskContext* context) = 0;
};

}

#endif