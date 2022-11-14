#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "linmath.h"

/*
class GLFWManager {
public:
    GLFWManager() {
        if (!glfwInit()) {
            std::cout << "glfwInit failed" << std::endl;
        }
        else {
            std::cout << "glfwInit passed" << std::endl;
        }
    }

    ~GLFWManager() {
        glfwTerminate();
    }
};

class GLFWWindow : public sandbox::Component {
public:
    GLFWWindow() : window(NULL) {
        addType<GLFWWindow>();
        addAttribute(new sandbox::TypedAttributeRef<int>("width", width, 640));
        addAttribute(new sandbox::TypedAttributeRef<int>("height", height, 480));
        addAttribute(new sandbox::TypedAttributeRef<std::string>("title", title, "My Title"));
    }

    ~GLFWWindow() {
        if (window) {
            glfwDestroyWindow(window);
        }
    }

    void init() {
        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (!window) {
            // Window or OpenGL context creation failed
            std::cout << "window failed" << std::endl;
        }
		glfwSetWindowUserPointer(window, this);
		glfwSetWindowSizeCallback(window, GLFWWindow::glfw_size_callback);
    	glfwSetCursorPosCallback(window, GLFWWindow::glfw_cursor_position_callback);
    	glfwSetMouseButtonCallback(window, GLFWWindow::glfw_mouse_button_callback);
        glfwSwapBuffers(window);
        glfwSwapInterval(1);
        glfwPollEvents();
    }

private:

    void sizeCallback(GLFWwindow* window, int width, int height) {
        std::cout << "sizeCallback" << std::endl;
  		this->width = width;
  		this->height = height;
  	}

    void cursorPositionCallback(GLFWwindow* window, float xpos, float ypos) {
    	std::cout << xpos/width << ", " << 1.0-ypos/height << std::endl;
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        std::cout << button << ", " << action << ", " << mods << std::endl;
    }

    static void glfw_size_callback(GLFWwindow* window, int width, int height) {
        std::cout << "sizeCallback" << std::endl;
		((GLFWWindow*)(glfwGetWindowUserPointer(window)))->sizeCallback(window, width, height);
	}

    static void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
        std::cout <<"pos" << std::endl;
    	((GLFWWindow*)(glfwGetWindowUserPointer(window)))->cursorPositionCallback(window, (float)xpos, (float)ypos);
	}

    static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		((GLFWWindow*)(glfwGetWindowUserPointer(window)))->mouseButtonCallback(window, button, action, mods);
	}

private:
    GLFWwindow* window;
    int width, height;
    std::string title;
};

class GLFWInitContext : public sandbox::RecursiveTask {
public:
    void runEntity(sandbox::Entity& entity, sandbox::TaskContext* context) {
        using namespace sandbox;
        const std::vector<Component*>& components = entity.getComponents();
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            GLFWWindow* window = (*it)->asType<GLFWWindow>();
            if (window) {
                window->init();
            }
        }
    }
};

class GLFWLoop : public sandbox::Task {
public:
    void run(sandbox::Entity& entity, sandbox::TaskContext* context) {
        glfwPollEvents();
    }
};

extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;

        static GLFWManager manager;
		
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<GLFWWindow>("GLFWWindow");
            ec->tasks().addType<GLFWInitContext>("GLFWInitContext");
            ec->tasks().addType<GLFWLoop>("GLFWLoop");
        }
	}

}

*/


typedef struct Vertex
{
    vec2 pos;
    vec3 col;
} Vertex;

static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
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

class OpenGLTest: public sandbox::Component {
public:
    OpenGLTest() {
        addType<OpenGLTest>();
    }

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
        glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*) offsetof(Vertex, pos));
        glEnableVertexAttribArray(vcol_location);
        glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*) offsetof(Vertex, col));
    }

    void run() {

        
        // NOTE: OpenGL error checks have been omitted for brevity

        int width = 640;
        int height = 480;
        const float ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4 m, p, mvp;
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);
  
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

