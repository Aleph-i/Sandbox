#ifndef SANDBOX_COMPONENT_H_
#define SANDBOX_COMPONENT_H_

namespace sandbox {

class Entity;

class Component {
friend class Entity;
public:
    Component() : entity(nullptr) {}
    virtual ~Component() {}
    Entity* getEntity() { return entity; }

private:
    void setEntity(Entity& entity);
    Entity* entity;
};

}

#endif