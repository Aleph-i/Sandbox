#ifndef SANDBOX_OBJECT_OBJECT_H_
#define SANDBOX_OBJECT_OBJECT_H_

#include <string>
#include <vector>
#include <typeinfo>

namespace sandbox {

class ValueImpl {
public:
    virtual ~ValueImpl() {}

    virtual void* get(void* state, const std::type_info& type) const = 0;
};

class Value {
public:
    Value(const ValueImpl* impl, void* state) : impl(impl), state(state) {}
    Value(const Value& val) {
        this->impl = val.impl;
        this->state = val.state;
    }
    void operator=(const Value& val) {
        this->impl = val.impl;
        this->state = val.state;
    }

    template <typename T>
    T& get() {
        static const std::type_info& type = typeid(T);
        void* val = impl->get(state, type);
        if (val) {
            return *static_cast<T*>(val);
        }
        static T defaultVal = T();
        return defaultVal;
    }

private:
    const ValueImpl* impl;
    void* state;
};

class ObjectImpl {
public:
    virtual ~ObjectImpl() {}

    virtual Value getValue(void* state, const std::string& key) const = 0;
};



class Object {
public:
    Object(const ObjectImpl* impl, void* state) : impl(impl), state(state) {}
    Object(const Object& obj) {
        this->impl = obj.impl;
        this->state = obj.state;
    }
    void operator=(const Object& obj) {
        this->impl = obj.impl;
        this->state = obj.state;
    }

    Value operator[](const std::string& key) const { return impl->getValue(state, key); }

private:
    const ObjectImpl* impl;
    void* state;
};

}

#endif