#ifndef SANDBOX_COMPONENT_H_
#define SANDBOX_COMPONENT_H_

#include <typeinfo>
#include <vector>
#include <string>
#include <map>
#include "attribute.h"
#include "sandbox/object/object.h"
#include "sandbox/parameter_set.h"

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
        T* ptr = dynamic_cast<T*>(this);
		addType(type, ptr);
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

    virtual Attribute& operator[](const std::string& name) const {
        std::map<std::string, Attribute*>::const_iterator it = attributes.find(name);
        if (it != attributes.end()) {
            return *(it->second);
        }
        else {
            return NullAttribute::instance();
        }
    }

    virtual const std::vector<Attribute*>& getAttributes() { return attributeList; }

    template <typename T>
    Component& setAttribute(const std::string& name, T val) {
        (*this)[name].set<T>(val);
        return *this;
    }

protected:
    virtual void addType(const std::type_info& type, void* ptr) {
        types.push_back(&type);
        pointers.push_back(ptr);
    }

    virtual void addAttribute(Attribute* attribute) {
        attributes[attribute->getName()] = attribute;
        attributeList.push_back(attribute);
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
    std::vector<Attribute*> attributeList;
};

class ComponentProxy : public Component {
public:
    ComponentProxy(Component* component) : component(component) {}
    virtual ~ComponentProxy() {}

    virtual const std::vector<const std::type_info*>& getTypes() const { return component->getTypes(); }

    virtual Attribute& operator[](const std::string& name) { return component->operator[](name); }

    const std::vector<Attribute*>& getAttributes() { return component->getAttributes(); }

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

template <typename T, typename BASE>
class ComponentWrapper : public Component {
public:
    ComponentWrapper() : val() {
        addType<T>(&val);
        addType<BASE>(&val);
    }

private:
    T val;
};

}

#endif