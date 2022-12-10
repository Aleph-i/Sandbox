#ifndef SANDBOX_GRAPHICS_RENDERERABLE_H_
#define SANDBOX_GRAPHICS_RENDERERABLE_H_

#include "component.h"
#include <map>
#include <typeinfo>
#include <vector>

namespace sandbox {

class ItemStack {
public:
    virtual ~ItemStack() {}

    template <typename T>
    bool isType() {
        static const std::type_info& type = typeid(T);
        return isType(type);
    }

    template <typename T>
    void push(T val) {
        static const std::type_info& type = typeid(T);
        pushValue(&val, type);
    }

    template <typename T>
    T get() {
        static const std::type_info& type = typeid(T);
        if (isType<T>()) {
            void* val = getValue(type);
            if (val) {
                return *static_cast<T*>(val);
            }
        }

        return T();
    }

    virtual void pop() = 0;

protected:
    virtual bool isType(const std::type_info& type) = 0;
    virtual void* getValue(const std::type_info& type) = 0;
    virtual void pushValue(void* val, const std::type_info& type) = 0;
};

template <typename T>
class TypedItemStack : public ItemStack {
public:
    TypedItemStack(T defaultVal = T()) : defaultVal(defaultVal), type(typeid(T)) {}
    virtual void pop() {
        if (stack.size() > 0) {
            stack.pop_back();
        }
    }

protected:
    virtual bool isType(const std::type_info& t) {
        return type == t;

    }
    virtual void* getValue(const std::type_info& type) {
        if (isType(type) && stack.size() > 0) {
            return &stack[stack.size()-1];
        }

        return &defaultVal;
    }

    virtual void pushValue(void* val, const std::type_info& type) {
        if (isType(type)) {
            stack.push_back(*static_cast<T*>(val));
        }
    }

private:
    const std::type_info& type;
    std::vector<T> stack;
    T defaultVal;
};

class GenericItemStack : public ItemStack {
public:
    GenericItemStack() {}

    virtual void pop() {}

protected:
    virtual bool isType(const std::type_info& type) { return false; }
    virtual void* getValue(const std::type_info& type) { return NULL; }
    virtual void pushValue(void* val, const std::type_info& type) {}
};

class Renderable;
class ContextObject {
public:
    virtual ~ContextObject() {}
};

class RenderContext {
public:
    virtual ~RenderContext() {
        for (std::map<std::string, ItemStack*>::iterator it = itemStacks.begin(); it != itemStacks.end(); it++) {
            delete it->second;
        }

        for (std::map<const Renderable*, ContextObject*>::iterator it = contextObjects.begin(); it != contextObjects.end(); it++) {
            delete it->second;
        }
    }

    void addStack(const std::string& key, ItemStack* stack) {
        std::map<std::string, ItemStack*>::iterator it = itemStacks.find(key);
        if (it == itemStacks.end()) {
            itemStacks[key] = stack;
        }
        else {
            delete stack;
        }
    }

    virtual ItemStack& operator[](const std::string& key) const  {
        std::map<std::string, ItemStack*>::const_iterator it = itemStacks.find(key);
        if (it != itemStacks.end()) {
            return *it->second;
        }
        else {
            static GenericItemStack stack;
            return stack;
        }
    }

    void addContextObject(const Renderable* renderable, ContextObject* obj);

    ContextObject* getContextObject(const Renderable* renderable) const;

private:
    std::map<std::string, ItemStack*> itemStacks;
    std::map<const Renderable*, ContextObject*> contextObjects;
};

class Renderable {
public:
    virtual ~Renderable() {}

    virtual ContextObject* createContextObject() {
        return new ContextObject();
    }

    virtual void update(RenderContext& context) {
        ContextObject* obj = context.getContextObject(this);
        if (!obj) {
            obj = createContextObject();
            context.addContextObject(this, obj);
            updateContext(context);
        }
    }
    virtual void updateContext(RenderContext& context) = 0;
    virtual void startRender(const RenderContext& context) = 0;
    virtual void finishRender(const RenderContext& context) = 0;
};

template <typename T>
class ContextRenderable : public Renderable {
public:
    virtual ~ContextRenderable() {}

    ContextObject* createContextObject() {
        return new T();
    }

    /*T& getContextObject(const RenderContext& context) {
        return *static_cast<T*>(context.getContextObject(this));
    }*/

    T& getContextObject(const RenderContext& context) const {
        return *static_cast<T*>(context.getContextObject(this));
    }

    void updateContext(RenderContext& context) { updateContext(context, getContextObject(context)); }
    void startRender(const RenderContext& context) { startRender(context, getContextObject(context)); }
    void finishRender(const RenderContext& context) { finishRender(context, getContextObject(context)); }

protected:
    virtual void updateContext(RenderContext& context, T& contextObject) = 0;
    virtual void startRender(const RenderContext& context, T& contextObject) = 0;
    virtual void finishRender(const RenderContext& context, T& contextObject) = 0;
};

void RenderContext::addContextObject(const Renderable* renderable, ContextObject* obj) {
    contextObjects[renderable] = obj;
}

ContextObject* RenderContext::getContextObject(const Renderable* renderable) const {
        std::map<const Renderable*, ContextObject*>::const_iterator it = contextObjects.find(renderable);
        if (it != contextObjects.end()) {
            return it->second;
        }

        return NULL;
    }

}

#endif