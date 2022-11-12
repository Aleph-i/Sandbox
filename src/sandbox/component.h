#ifndef SANDBOX_COMPONENT_H_
#define SANDBOX_COMPONENT_H_

#include <typeinfo>
#include <vector>

namespace sandbox {

class Renderable {
public:
    virtual ~Renderable() {}
    virtual void Render() = 0;
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

    template<typename T>
	T* asType() {
        static const std::type_info& type = typeid(T);
        for (int i = 0; i < types.size(); i++) {
            if (*types[i] == type) {
                return static_cast<T*>(pointers[i]);
            }	
        }

        return NULL;
	}

protected:
    virtual void addType(const std::type_info& type, void* ptr) {
        types.push_back(&type);
        pointers.push_back(ptr);
    }

private:
    void setEntity(Entity& entity);
    Entity* entity;
    std::vector<const std::type_info*> types;
    std::vector<void*> pointers;
};

}

#endif