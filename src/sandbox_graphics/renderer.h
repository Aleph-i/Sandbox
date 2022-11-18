#ifndef SANDBOX_GRAPHICS_RENDERER_H_
#define SANDBOX_GRAPHICS_RENDERER_H_

#include "entity.h"
#include "component.h"
#include "renderable.h"

namespace sandbox {

class Renderer : public Component {
public:
    Renderer() {
        addType<Renderer>();
        addAttribute(new TypedAttributeRef< std::vector<Entity*> >("renderGroups", renderGroups));
    }

    void addRenderGroup(Entity& renderGroup) {
        renderGroups.push_back(&renderGroup);
    }

private:
    std::vector<Entity*> renderGroups;
};



}

#endif