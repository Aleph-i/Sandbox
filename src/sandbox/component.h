#ifndef SANDBOX_COMPONENT_H_
#define SANDBOX_COMPONENT_H_

#include <typeinfo>
#include <vector>
#include <string>
#include <map>
#include "attribute.h"
#include "sandbox/object/object.h"

namespace sandbox {

class Entity;

class Component {
friend class Entity;
friend class ComponentProxy;
public:
    Component() : entity(nullptr) {}
    virtual ~Component() {}
    Entity* getEntity() { return entity; }

    template<typename T>
	void addType() {
		static const std::type_info& type = typeid(T);
		addType(type, this);
	}

    template<typename T>
	void addType(void* ptr) {
		static const std::type_info& type = typeid(T);
		addType(type, ptr);
	}

    template <typename T>
	T* asType() {
        static const std::type_info& type = typeid(T);
        const std::vector<const std::type_info*>& componentTypes = getTypes();
        for (int i = 0; i < componentTypes.size(); i++) {
            if (*componentTypes[i] == type) {
                return static_cast<T*>(getPointers()[i]);
            }	
        }

        return NULL;
	}

	virtual const std::vector<const std::type_info*>& getTypes() const { return types; }

    virtual Attribute& operator[](const std::string& name) {
        return *attributes[name];
    }

protected:
    virtual void addType(const std::type_info& type, void* ptr) {
        types.push_back(&type);
        pointers.push_back(ptr);
    }

    virtual void addAttribute(Attribute* attribute) {
        attributes[attribute->getName()] = attribute;
    }

    virtual void update() {}

    virtual const std::vector<void*>& getPointers() const { return pointers; }

private:
    void updateComponent();
    void setEntity(Entity& entity);
    Entity* entity;
    std::vector<const std::type_info*> types;
    std::vector<void*> pointers;
    std::map<std::string, Attribute*> attributes;
};

class ComponentProxy : public Component {
public:
    ComponentProxy(Component* component) : component(component) {}
    virtual ~ComponentProxy() {}

    virtual const std::vector<const std::type_info*>& getTypes() const { return component->getTypes(); }

    virtual Attribute& operator[](const std::string& name) { return component->operator[](name); }

protected:
    virtual void addType(const std::type_info& type, void* ptr) {}

    virtual void addAttribute(Attribute* attribute) {}

    virtual void update() {}

    virtual const std::vector<void*>& getPointers() const { return component->getPointers(); }

private:
    Component* component;
};

class CallbackComponent : public Component {
public:
    CallbackComponent() {
        addType<CallbackComponent>();
    }
    virtual ~CallbackComponent() {}

    virtual void run(sandbox::Object& params, sandbox::Object& returnVal) = 0;
};

}

#endif