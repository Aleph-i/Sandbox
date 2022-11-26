#ifndef SANDBOX_PICO_JSON_OBJECT_H_
#define SANDBOX_PICO_JSON_OBJECT_H_

#include "sandbox/object/object.h"
#include "picojson.h"

class PicoJsonValueImpl : public sandbox::ValueImpl {
private:
    void* get(void* state, const std::type_info& type) const {
        picojson::value& val = *static_cast<picojson::value*>(state);
        if (type == typeid(double)) {
            std::cout << "double" << std::endl;
            return &val.get<double>();
        }

        return NULL;
    }

public:
    static const PicoJsonValueImpl& instance() {
        static PicoJsonValueImpl impl;
        return impl;
    }
};

class PicoJsonValue : public sandbox::Value {
public:
    PicoJsonValue(picojson::value& val) : sandbox::Value(&(PicoJsonValueImpl::instance()), &val) {}
};

class PicoJsonObjectImpl : public sandbox::ObjectImpl {
private:
    sandbox::Value getValue(void* state, const std::string& key) const {
        picojson::object& obj = *static_cast<picojson::object*>(state);
        return PicoJsonValue(obj[key]);
    }

public:
    static const PicoJsonObjectImpl& instance() {
        static PicoJsonObjectImpl impl;
        return impl;
    }
};

class PicoJsonObject : public sandbox::Object {
public:
    PicoJsonObject(picojson::object& obj) : sandbox::Object(&(PicoJsonObjectImpl::instance()), &obj) {}
};


#endif