#ifndef SANDBOX_GRAPHICS_RENDERERABLE_H_
#define SANDBOX_GRAPHICS_RENDERERABLE_H_

#include "component.h"

namespace sandbox {

class RenderContext {
public:
    virtual ~RenderContext() {}
    virtual void push() {}
    virtual void pop() {}
};

class SimpleRenderContext : public RenderContext {
public:
    SimpleRenderContext(Entity& scene) : scene(scene) {}

private:
    Entity& scene;
};

class Rendererable {
public:
    virtual ~Rendererable() {}

    virtual void render(RenderContext& context) = 0;
};

}

#endif