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

#include <fstream>
#include <sstream>

/*
std::string instr = "";

	std::ifstream fIn;
	fIn.open(file->getPath().c_str(),std::ios::in);

	if (fIn) {
		std::stringstream ss;
		ss << fIn.rdbuf();
		fIn.close();

		instr =  ss.str();
	}
    */

/*
	ifstream inFile(fileName, ios::in);
	if (!inFile)
	{
		return fileName;
	}

	std::stringstream ss;
	ss << inFile.rdbuf();
	inFile.close();
	return ss.str();
*/

struct OpenGLShaderData : public sandbox::ContextObject {
    GLuint shader;
};

class OpenGLShader : public sandbox::Component, public sandbox::ContextRenderable<OpenGLShaderData> {
public:
    OpenGLShader() : isLoaded(false), shaderText("") {
        addType<OpenGLShader>();
        addType<sandbox::Renderable>(static_cast<sandbox::Renderable*>(this));
        addAttribute(new sandbox::TypedAttributeRef<std::string>("shaderText", shaderText));
        addAttribute(new sandbox::TypedAttributeRef<std::string>("filePath", filePath));
        addAttribute(new sandbox::TypedAttributeRef<std::string>("shaderType", shaderType));
    }

    void update() {
        if (!isLoaded) {
            if (shaderText.length() == 0 && filePath.length() > 0) {
                std::ifstream inFile(filePath, std::ios::in);
                std::stringstream ss;
                ss << inFile.rdbuf();
                inFile.close();
                shaderText = ss.str();
            }

            if (shaderType == "vertex") {
                type = GL_VERTEX_SHADER;
            }
            else if (shaderType == "fragment") {
                type = GL_FRAGMENT_SHADER;
            }

            isLoaded = true;
        }
    }

    void updateContext(sandbox::RenderContext& context, OpenGLShaderData& shaderData) {
        shaderData.shader = glCreateShader(type);
        const char* text = shaderText.c_str();
        glShaderSource(shaderData.shader, 1, &text, NULL);
        glCompileShader(shaderData.shader);
        std::cout << "Shader id: " << shaderData.shader << std::endl;
        std::cout << text << std::endl;
    }

    virtual void startRender(const sandbox::RenderContext& context, OpenGLShaderData& shaderData) {

    }

    virtual void finishRender(const sandbox::RenderContext& context, OpenGLShaderData& shaderData) {

    }

private:
    bool isLoaded;
    std::string filePath;
    std::string shaderType;
    std::string shaderText;
    GLuint type;
};

struct OpenGLShaderProgramData : public sandbox::ContextObject {
    GLuint program;
};

class OpenGLShaderProgram : public sandbox::Component, public sandbox::ContextRenderable<OpenGLShaderProgramData> {
public:
    OpenGLShaderProgram() : isLoaded(false) {
        addType<OpenGLShaderProgram>();
        addType<sandbox::Renderable>(static_cast<sandbox::Renderable*>(this));
    }

    ~OpenGLShaderProgram() {
    }

    void update() {
        if (!isLoaded) {
            shaders = getEntity()->getComponents<OpenGLShader>();

            isLoaded = true;
        }
    }

    void test() {
        std::cout << "Shader Prog" << " " << this << " " << static_cast<sandbox::Renderable*>(this) << " " << static_cast< sandbox::ContextRenderable<OpenGLShaderProgramData>* >(this) << std::endl;
    }

    void updateContext(sandbox::RenderContext& context, OpenGLShaderProgramData& shaderData) {
        shaderData.program = glCreateProgram();
        std::cout << "Shader Prog" << shaderData.program << " " << &shaderData << " " << &context << " " << this << std::endl;
        std::cout << this << std::endl;
        for (int i = 0; i < shaders.size(); i++) {
            static_cast<sandbox::Renderable*>(shaders[i])->update(context);
            std::cout << "Shader ID: " << shaders[i]->getContextObject(context).shader << std::endl;
            glAttachShader(shaderData.program, shaders[i]->getContextObject(context).shader);
        }

        glLinkProgram(shaderData.program);

        GLint mvp_location = glGetUniformLocation(shaderData.program, "MVP");
        GLint vpos_location = glGetAttribLocation(shaderData.program, "vPos");
        GLint vcol_location = glGetAttribLocation(shaderData.program, "vCol");

        std::cout << "a " <<  mvp_location << std::endl;
        std::cout << vpos_location << std::endl;
        std::cout << vcol_location << std::endl;
    }

    virtual void startRender(const sandbox::RenderContext& context, OpenGLShaderProgramData& shaderData) {

    }

    virtual void finishRender(const sandbox::RenderContext& context, OpenGLShaderProgramData& shaderData) {

    }

private:
    bool isLoaded;
    std::vector<OpenGLShader*> shaders;
};

class OpenGLShaderCommand : public sandbox::Component, public sandbox::Renderable {
public:
    OpenGLShaderCommand() : isLoaded(false), shaderProgram(NULL) {
        addType<OpenGLShaderCommand>();
        addType<sandbox::Renderable>(static_cast<sandbox::Renderable*>(this));
        addAttribute(new sandbox::TypedAttributeRef<sandbox::Entity*>("shader", shader));
    }

    ~OpenGLShaderCommand() {
    }

    void update() {
        if (!isLoaded) {
            shaderProgram = shader->getComponent<OpenGLShaderProgram>();

            isLoaded = true;
        }
    }

    void updateContext(sandbox::RenderContext& context) {
        static_cast<sandbox::Renderable*>(shaderProgram)->update(context);
    }

    virtual void startRender(const sandbox::RenderContext& context) {
        //std::cout << "use shader" << std::endl;
        glUseProgram(shaderProgram->getContextObject(context).program);
        //shaderProgram->test();
        //std::cout << shaderProgram->getContextObject(context).program << " " << &shaderProgram->getContextObject(context) << " " << &context << " " << shaderProgram << std::endl;
        //std::cout << static_cast< sandbox::ContextRenderable<OpenGLShaderProgramData>* >(shaderProgram) << std::endl;
        //exit(0);
    }

    virtual void finishRender(const sandbox::RenderContext& context) {
        glUseProgram(0);
        //std::cout << "release shader" << std::endl;
    }

private:
    bool isLoaded;
    sandbox::Entity* shader;
    OpenGLShaderProgram* shaderProgram;
};

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


        glGenVertexArrays(1, &vertex_array);

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


        std::cout << mvp_location << std::endl;
        std::cout << vpos_location << std::endl;
        std::cout << vcol_location << std::endl;

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

        //glUseProgram(program);

        //GLuint vertex_array;
        glBindVertexArray(vertex_array);
        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*) offsetof(Vertex, pos));
        glEnableVertexAttribArray(vcol_location);
        glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*) offsetof(Vertex, col));

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
            ec->components().addType<OpenGLShaderProgram>("OpenGLShaderProgram");
            ec->components().addType<OpenGLShader>("OpenGLShader");
            ec->components().addType<OpenGLShaderCommand>("OpenGLShaderCommand");
            ec->tasks().addType<OpenGLInit>("OpenGLInit");
            ec->tasks().addType<OpenGLRun>("OpenGLRun");
        }
	}

}

