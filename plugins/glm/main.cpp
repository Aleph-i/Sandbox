#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox_graphics/renderable.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GlmCamera : public sandbox::Component, public sandbox::Renderable {
public:
    GlmCamera() {
        addType<GlmCamera>();
        addType<sandbox::Renderable>(static_cast<sandbox::Renderable*>(this));
        t = 0.0;
    }

    ~GlmCamera() {}

    void update() {


        //
        //view = glm::mat4(1.0);
    }

    virtual void updateContext(sandbox::RenderContext& context) {
        context.addStack("viewMatrix", new sandbox::TypedItemStack<glm::mat4>());
        context.addStack("projectionMatrix", new sandbox::TypedItemStack<glm::mat4>());
    }

    virtual void startRender(const sandbox::RenderContext& context) {
        t+= 0.0001;
        view = glm::lookAt(
		    //glm::vec3(4*sin(t),3,3), // Camera is at (4,3,3), in World Space
		    glm::vec3(4.0f,3,3), // Camera is at (4,3,3), in World Space
		    glm::vec3(0,0,0), // and looks at the origin
		    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		    );

        //view = glm::rotate(view, (float)t, glm::vec3(1.0, 0.0, 0.0));

        int width = context["width"].get<int>();
        int height = context["height"].get<int>();
        const float ratio = width / (float) height;

        context["viewMatrix"].push<glm::mat4>(view);
        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        //glm::mat4 projection = glm::ortho(-1.f, 1.f, 1.f, -1.f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);
        context["projectionMatrix"].push<glm::mat4>(projection);
        
    }

    virtual void finishRender(const sandbox::RenderContext& context) {
        context["viewMatrix"].pop();
        context["projectionMatrix"].pop();

    }

private:
    glm::mat4 view;
    double t;
};


extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<GlmCamera>("GlmCamera");
        }
	}
}