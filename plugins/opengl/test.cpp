#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox_graphics/renderable.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

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

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec3 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";

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
    }
    bool dir;

    ~OpenGLTest() {
    }


    void init() {
        //GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
        glCompileShader(vertex_shader);

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
        glCompileShader(fragment_shader);

        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        mvp_location = glGetUniformLocation(program, "MVP");
        vpos_location = glGetAttribLocation(program, "vPos");
        vcol_location = glGetAttribLocation(program, "vCol");

        //GLuint vertex_array;
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);
        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*) offsetof(Vertex, pos));
        glEnableVertexAttribArray(vcol_location);
        glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*) offsetof(Vertex, col));
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
        model = glm::rotate(model, (float) glfwGetTime()*(dir?1.0f:-1.0f), glm::vec3(0,0,1.0));
        glm::mat4 view = context["viewMatrix"].get<glm::mat4>();
        glm::mat4 proj = context["projectionMatrix"].get<glm::mat4>();
        glm::mat4 modelViewProjection = proj*view*model;

        glUseProgram(program);
        //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) glm::value_ptr(modelViewProjection));
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    virtual void finishRender(const sandbox::RenderContext& context) {
    }

private:
        GLuint vertex_buffer, vertex_shader, fragment_shader, program;
        GLint mvp_location, vpos_location, vcol_location;

        GLuint vertex_array;
};

class OpenGLRun : public sandbox::RecursiveTask {
public:
    void runEntity(sandbox::Entity& entity, sandbox::TaskContext* context) {
        using namespace sandbox;
        const std::vector<Component*>& components = entity.getComponents();
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            OpenGLTest* test = (*it)->asType<OpenGLTest>();
            if (test) {
                test->run();
            }
        }
    }
};

class OpenGLInit : public sandbox::RecursiveTask {
public:
    void runEntity(sandbox::Entity& entity, sandbox::TaskContext* context) {
        using namespace sandbox;
        const std::vector<Component*>& components = entity.getComponents();
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            OpenGLTest* test = (*it)->asType<OpenGLTest>();
            if (test) {
                test->init();
            }
        }
    }
};

extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;

        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<OpenGLTest>("OpenGLTest");
            ec->tasks().addType<OpenGLInit>("OpenGLInit");
            ec->tasks().addType<OpenGLRun>("OpenGLRun");
        }
	}

}

