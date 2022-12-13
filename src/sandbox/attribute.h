#ifndef SANDBOX_ATTRIBUTE_H_
#define SANDBOX_ATTRIBUTE_H_

#include <typeinfo>
#include <vector>
#include <string>
#include <iostream>

namespace sandbox {

class Attribute {
public:
    Attribute(const std::string& name) : name(name) {}
    virtual ~Attribute() {}
    
    template <typename T>
    T& get() {
        static const std::type_info& type = typeid(T);
        if (type == getType()) {
            return *static_cast<T*>(getValue()); 
        }
        static T defaultValue = T();
        std::cout << "Returning default: " << name << std::endl;
        return defaultValue;
    }

    template <typename T>
    void set(T val) {
        static const std::type_info& type = typeid(T);
        if (type == getType()) {
            setValue(&val);
        }
    }

    template <typename T>
    bool isType() {
        static const std::type_info& type = typeid(T);
        return type == getType();
    }

    const std::string& getName() { return name; }

protected:
    virtual void* getValue() = 0;
    virtual void setValue(void* val) = 0;
    virtual const std::type_info& getType() = 0;
    std::string name;
};

class NullAttribute : public Attribute {
private:
    NullAttribute() : Attribute("") {}

public:
    static Attribute& instance() {
        static NullAttribute nullAttribute;
        return nullAttribute;
    }

protected:
    virtual void* getValue() { return NULL; }
    virtual void setValue(void* val) {}
    virtual const std::type_info& getType() {
        static const std::type_info& type = typeid(void*);
        return type;
    }
};

template <typename T>
class TypedAttribute : public Attribute {
public:
    TypedAttribute(const std::string& name, T defaultValue = T()) : Attribute(name), val(defaultValue) {}

protected:
    virtual void* getValue() { return &val; }
    virtual void setValue(void* val) { this->val = *static_cast<T*>(val); }
    virtual const std::type_info& getType() {
        static const std::type_info& type = typeid(T);
        return type;
    }

private:
    T val;
};

template <typename T>
class TypedAttributeRef : public Attribute {
public:
    TypedAttributeRef(const std::string& name, T& val, T defaultValue = T()) : Attribute(name), val(val) {
        this->val = defaultValue;
    }

protected:
    virtual void* getValue() { return &val; }
    virtual void setValue(void* val) { this->val = *static_cast<T*>(val); }
    virtual const std::type_info& getType() {
        static const std::type_info& type = typeid(T);
        return type;
    }

private:
    T& val;
};

}

#endif