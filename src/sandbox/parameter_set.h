#ifndef SANDBOX_PARAMETER_SET_H_
#define SANDBOX_PARAMETER_SET_H_

#include <map>
#include <iostream>

namespace sandbox {

class Parameter {
public:
    Parameter() {}
    virtual ~Parameter() {}

    template<typename T> 
    const T get(const T& defaultValue = T()) const {
        const void* val = getValue();
        if (val) {
            return *static_cast<const T*>(val);
        }
        else {
            return defaultValue;
        }
    }

    virtual const void* getValue() const { return NULL; }
};

template<typename T>
class TypedParameter : public Parameter {
public:
    TypedParameter(const T& val) : val(val) {}

    const void* getValue() const {
        return &val;
    }

private:
    T val;
};

class ParameterSet {
public:
    ParameterSet() {}
    ~ParameterSet() {
        std::cout << "Destroy params" << std::endl;
        for (std::map<std::string, Parameter*>::iterator it = params.begin(); it != params.end(); it++) {
            delete it->second;
        }
    }

    Parameter& operator[](const std::string& name) const {
        std::cout << "get " << name << std::endl;
        std::map<std::string, Parameter*>::const_iterator it = params.find(name);
        if (it != params.end()) {
            return *params.find(name)->second;
        }

        static Parameter defaultParameter;
        return defaultParameter;
    }

    template <typename T>
    ParameterSet& add(const std::string& name, const T& param) {
        std::cout << "add " << name << std::endl;
        params[name] = new TypedParameter<T>(param);
        return *this;
    }

private:
    std::map<std::string, Parameter*> params;
};

}


#endif