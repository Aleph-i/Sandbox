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


//-------------------------------------------

template <typename T>
class MapValueImpl : public sandbox::ValueImpl {
private:
    void* get(void* state, const std::type_info& type) const {
        T& val = *static_cast<T*>(state);
        return &val;
    }

public:
    static const MapValueImpl& instance() {
        static MapValueImpl impl;
        return impl;
    }
};

template <typename T>
class MapValue : public sandbox::Value {
public:
    MapValue(T& val) : sandbox::Value(&(MapValueImpl<T>::instance()), &val) {}
};

template <typename T>
class MapObjectImpl : public sandbox::ObjectImpl {
private:
    sandbox::Value getValue(void* state, const std::string& key) const {
        std::map<std::string, T*>& obj = *static_cast<std::map<std::string, T*>*>(state);
        return MapValue<T>(*obj[key]);
    }

public:
    static const MapObjectImpl& instance() {
        static MapObjectImpl<T> impl;
        return impl;
    }
};

template <typename T>
class MapObject : public sandbox::Object {
public:
    MapObject() : sandbox::Object(&(MapObjectImpl<T>::instance()), &mappedValues) {}
    void mapValue(const std::string& key, T& val) {
        mappedValues[key] = &val;
    }
private:
    std::map<std::string, T*> mappedValues;
};

}

#endif