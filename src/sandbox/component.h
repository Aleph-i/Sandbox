#ifndef SANDBOX_COMPONENT_H_
#define SANDBOX_COMPONENT_H_

#include <typeinfo>
#include <vector>
#include <string>
#include <map>
#include "attribute.h"
#include "sandbox/object/object.h"

namespace sandbox {

class Component;

class ComponentCallback {
public:
    virtual ~ComponentCallback() {}

    virtual void run(Component& component, sandbox::Object& params, sandbox::Object& returnVal) = 0;
};

class Entity;

class Component {
friend class Entity;
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
	virtual const std::vector<const std::type_info*>& getTypes() { return types; }

    template <typename T>
	T* asType() {
        static const std::type_info& type = typeid(T);
        for (int i = 0; i < types.size(); i++) {
            if (*types[i] == type) {
                return static_cast<T*>(pointers[i]);
            }	
        }

        return NULL;
	}

    virtual Attribute& operator[](const std::string& name) {
        return *attributes[name];
    }

    virtual void addCallback(const std::string& name, ComponentCallback* callback) {
        callbacks[name].push_back(callback);
    }

protected:
    virtual void createCallback(const std::string& name) {
        std::map<std::string, std::vector<ComponentCallback*> >::iterator it = callbacks.find(name);
        if (it == callbacks.end()) {
            callbacks[name] = std::vector<ComponentCallback*>();
        }
    }

    virtual void runCallback(const std::string& name, sandbox::Object& params, sandbox::Object& returnVal) {
        std::vector<ComponentCallback*>& callbackList = callbacks[name];
        for (int i = 0; i < callbackList.size(); i++) {
            callbackList[i]->run(*this, params, returnVal);
        }
    }

    virtual void addType(const std::type_info& type, void* ptr) {
        types.push_back(&type);
        pointers.push_back(ptr);
    }

    virtual void addAttribute(Attribute* attribute) {
        attributes[attribute->getName()] = attribute;
    }

    virtual void update() {}

private:
    void updateComponent();
    void setEntity(Entity& entity);
    Entity* entity;
    std::vector<const std::type_info*> types;
    std::vector<void*> pointers;
    std::map<std::string, Attribute*> attributes;
    std::map<std::string, std::vector<ComponentCallback*> > callbacks;
};

}

#endif