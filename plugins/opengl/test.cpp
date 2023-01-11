#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox_graphics/renderable.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "shader/opengl_shader.h"

#include "linmath.h"

#include "glm/glm.hpp"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef struct Vertex
{
    vec3 pos;
    vec3 col;
} Vertex;

static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f, 0.0f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f, 0.0f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f, 0.0f }, { 0.f, 0.f, 1.f } }
};

struct GeometryArray {
    int size;
    const void* data;
};

class SimpleTriangle : public sandbox::Component {
public:
    SimpleTriangle() {
        addType<SimpleTriangle>();
        addAttribute(new sandbox::TypedAttributeRef<GeometryArray>("vertices", verts));
    }

    void update() {
        verts.size = sizeof(vertices);
        verts.data = vertices;
    }

    GeometryArray verts;
};

class OpenGLVertexBuffer : public sandbox::Component, public sandbox::ContextRenderable<OpenGLObject> {
public:
    OpenGLVertexBuffer() : isLoaded(false) {
        addType<OpenGLVertexBuffer>();
        addType<sandbox::Renderable>(static_cast<sandbox::Renderable*>(this));
        addAttribute(new sandbox::TypedAttributeRef<sandbox::Entity*>("mesh", mesh));
        addAttribute(new sandbox::TypedAttributeRef<std::string>("arrayName", arrayName, "vertices"));
    }

    void update() {
        if (!isLoaded) {
            if (mesh) {
                arr = (*mesh)[arrayName].get<GeometryArray>();

                isLoaded = true;
            }
        }
    }

    void updateContext(sandbox::RenderContext& context, OpenGLObject& vertexBuffer) {
        glGenBuffers(1, &vertexBuffer.id);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.id);
        glBufferData(GL_ARRAY_BUFFER, arr.size, arr.data, GL_STATIC_DRAW);
    }

    virtual void startRender(const sandbox::RenderContext& context, OpenGLObject& vertexBuffer) {
    }

    virtual void finishRender(const sandbox::RenderContext& context, OpenGLObject& vertexBuffer) {
    }

    sandbox::ContextObject* createContextObject() {
        return new VertexBufferObject();
    }

private:
    struct VertexBufferObject : public OpenGLObject {
        virtual ~VertexBufferObject() {
            glDeleteBuffers(1, &id);
        }
    };

    bool isLoaded;
    sandbox::Entity* mesh;
    std::string arrayName;
    GeometryArray arr;
};


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

class OpenGLTest: public sandbox::Component, public sandbox::Renderable {
public:
    OpenGLTest() {
        addType<OpenGLTest>();
        addType<sandbox::Renderable>(static_cast<sandbox::Renderable*>(this));
        addAttribute(new sandbox::TypedAttributeRef<bool>("dir", dir, true));
        addAttribute(new sandbox::TypedAttributeRef<sandbox::Entity*>("mesh", mesh));
    }
    bool dir;
    sandbox::Entity* mesh;

    ~OpenGLTest() {
    }


    void init() {
        //GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        GeometryArray& arr = (*mesh)["vertices"].get<GeometryArray>();
        glBufferData(GL_ARRAY_BUFFER, arr.size, arr.data, GL_STATIC_DRAW);


        glGenVertexArrays(1, &vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void run() {
    }

    virtual void updateContext(sandbox::RenderContext& context) {
        init();
    }
    virtual void startRender(const sandbox::RenderContext& context) {
        // NOTE: OpenGL error checks have been omitted for brevity

        int width = context["width"].get<int>();
        int height = context["height"].get<int>();
        const float ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4 m, p, mvp;
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime()*(dir?1.0:-1.0));
        //glm::radians(45.0f), aspectRatio, 0.1f, 100.0f
        //mat4x4_perspective(p, 0.39269875, ratio, 0.1f, 100.0f);
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glm::mat4 model(1.0);
        model = glm::translate(model, glm::vec3(1,0,0)*sin((float) glfwGetTime()));
        model = glm::rotate(model, (float) glfwGetTime()*(dir?1.0f:-1.0f), glm::vec3(0,0,1.0));
        glm::mat4 view = context["viewMatrix"].get<glm::mat4>();
        glm::mat4 proj = context["projectionMatrix"].get<glm::mat4>();
        glm::mat4 modelViewProjection = proj*view*model;

        //glUseProgram(program);

        /*mvp_location = glGetUniformLocation(program, "MVP");
        vpos_location = glGetAttribLocation(program, "vPos");
        vcol_location = glGetAttribLocation(program, "vCol");*/

        //GLuint vertex_array;
        if (!loaded) {
            OpenGLShaderProgram* prog = context["shaderProgram"].get<OpenGLShaderProgram*>();
            program = prog->getContextObject(context).id;

            mvp_location = prog->getUniformLocation("MVP");
            vpos_location = prog->getAttribLocation("vPos");
            vcol_location = prog->getAttribLocation("vCol");

            glUseProgram(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glBindVertexArray(vertex_array);
            glEnableVertexAttribArray(vpos_location);
            glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), (void*) offsetof(Vertex, pos));
            glEnableVertexAttribArray(vcol_location);
            glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), (void*) offsetof(Vertex, col));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            glUseProgram(program);

            loaded = true;
        }

        //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) glm::value_ptr(modelViewProjection));
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //std::cout << "draw triangles" << std::endl;
    }

    virtual void finishRender(const sandbox::RenderContext& context) {
    }

private:
        GLuint vertex_buffer, vertex_shader, fragment_shader, program;
        GLint mvp_location, vpos_location, vcol_location;
        bool loaded = false;

        GLuint vertex_array;
};

namespace sandbox::opengl {



}

extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;

        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<OpenGLTest>("OpenGLTest");
            ec->components().addType<OpenGLShaderProgram>("OpenGLShaderProgram");
            ec->components().addType<OpenGLShader>("OpenGLShader");
            ec->components().addType<OpenGLShaderCommand>("OpenGLShaderCommand");
            ec->components().addType<SimpleTriangle>("SimpleTriangle");
        }
	}

}

