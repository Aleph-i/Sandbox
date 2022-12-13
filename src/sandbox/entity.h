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

    Attribute& operator[](const std::string& name) const;

    void update();

    template <typename T>
    T* getComponent() {
        const std::vector<Component*>& components = getComponents();
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            T* component = (*it)->asType<T>();
            if (component) {
                return component;
            }
        }

        return NULL;
    }

    template <typename T>
    T* getComponentFromAbove() {
        Entity* entity = this;
        while (entity) {
            T* component = entity->getComponent<T>();
            if (component) {
                return component;
            }
            entity = entity->getParent();
        }

        return NULL;
    }

    template <typename T>
    std::vector<T*> getComponents() {
        const std::vector<Component*>& components = getComponents();
        std::vector<T*> typedComponents;
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            T* component = (*it)->asType<T>();
            if (component) {
                typedComponents.push_back(component);
            }
        }

        return typedComponents;
    }

/*    void addCallback(const std::string& name, ComponentCallback* callback) {
        callbacks[name].push_back(callback);
    }

    void createCallback(const std::string& name) {
        std::map<std::string, std::vector<ComponentCallback*> >::iterator it = callbacks.find(name);
        if (it == callbacks.end()) {
            callbacks[name] = std::vector<ComponentCallback*>();
        }
    }

    void runCallback(const std::string& name, sandbox::Object& params, sandbox::Object& returnVal) {
        std::vector<ComponentCallback*>& callbackList = callbacks[name];
        for (int i = 0; i < callbackList.size(); i++) {
            callbackList[i]->run(*this, params, returnVal);
        }
    }
    */

private:
    void removeComponent(Component* component);
    void removeChild(Entity* child);
    void setParent(Entity* parent) { this->parent = parent; }

    std::vector<Component*> components;
    std::vector<Entity*> children;
    Entity* parent;
    std::string name;
    //std::map<std::string, std::vector<ComponentCallback*> > callbacks;
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

template <typename T>
class TypedRecursiveTask2 : public sandbox::Task {
public:
    void run(Entity& entity, TaskContext* context) {
        startEntity(entity, context);

        if (context) {
            context->push();
        }
        const std::vector<Entity*>& entities = entity.getChildren();
        for (std::vector<Entity*>::const_iterator it = entities.begin(); it != entities.end(); it++) {
            (*it)->runTask(*this, context);
        }

        if (context) {
            context->pop();
        }

        finishEntity(entity, context);
    }

    void startEntity(sandbox::Entity& entity, sandbox::TaskContext* context) {
        using namespace sandbox;
        const std::vector<Component*>& components = entity.getComponents();
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            T* component = (*it)->asType<T>();
            if (component) {
                startComponent(*component, context);
            }
        }
    }

    void finishEntity(sandbox::Entity& entity, sandbox::TaskContext* context) {
        using namespace sandbox;
        const std::vector<Component*>& components = entity.getComponents();
        for (std::vector<Component*>::const_reverse_iterator it = components.rbegin(); it != components.rend(); it++) {
            T* component = (*it)->asType<T>();
            if (component) {
                finishComponent(*component, context);
            }
        }
    }

    virtual void startComponent(T& component, sandbox::TaskContext* context) = 0;
    virtual void finishComponent(T& component, sandbox::TaskContext* context) = 0;
};

}

#endif