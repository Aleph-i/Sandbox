#ifndef SANDBOX_FACTORY_OBJECT_FACTORY_H_
#define SANDBOX_FACTORY_OBJECT_FACTORY_H_

#include <vector>

namespace sandbox {

template <typename BASE>
class ObjectFactory {
public:
    virtual ~ObjectFactory() {}
    virtual BASE* create(const std::string& typeName) = 0;
};

template <typename T, typename BASE>
class TypedObjectFactory : public ObjectFactory<BASE> {
public:
    TypedObjectFactory(const std::string& typeName) : typeName(typeName) {}
    virtual ~TypedObjectFactory() {}
    BASE* create(const std::string& typeName) { 
        if (typeName == this->typeName) {
            return new T();
        }
        else {
            return NULL;
        } 
    }
private:
    std::string typeName;
};

template <typename BASE>
class CompositeObjectFactory {
public:
    virtual ~CompositeObjectFactory() {}

    template <typename T>
    void addType(const std::string& typeName) { addFactory(new TypedObjectFactory<T, BASE>(typeName)); }
    void addFactory(ObjectFactory<BASE>* factory) {
        factories.push_back(factory);
    }
    virtual BASE* create(const std::string& typeName) {
        for (int i = 0; i < factories.size(); i++) {
            BASE* item = factories[i]->create(typeName);
            if (item) {
                return item;
            }
        }

        return NULL;
    }

private:
    std::vector< ObjectFactory<BASE>* > factories;
};

}

#endif