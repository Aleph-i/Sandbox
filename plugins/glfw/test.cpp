#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        int version = gladLoadGL(glfwGetProcAddress);
        printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (!window) {
            // Window or OpenGL context creation failed
            std::cout << "window failed" << std::endl;
        }
		glfwSetWindowUserPointer(window, this);
		glfwSetWindowSizeCallback(window, GLFWWindow::glfw_size_callback);
    	glfwSetCursorPosCallback(window, GLFWWindow::glfw_cursor_position_callback);
    	glfwSetMouseButtonCallback(window, GLFWWindow::glfw_mouse_button_callback);

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
        glfwSwapInterval(1);
    }

    void swapBuffers() {
        glfwSwapBuffers(window);
    }

    void makeCurrent() {
        glfwMakeContextCurrent(window);
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

class GLFWSwapBuffers : public sandbox::RecursiveTask {
public:
    void runEntity(sandbox::Entity& entity, sandbox::TaskContext* context) {
        using namespace sandbox;
        const std::vector<Component*>& components = entity.getComponents();
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            GLFWWindow* window = (*it)->asType<GLFWWindow>();
            if (window) {
                window->swapBuffers();
            }
        }
    }
};

class GLFWMakeCurrent : public sandbox::RecursiveTask {
public:
    void runEntity(sandbox::Entity& entity, sandbox::TaskContext* context) {
        using namespace sandbox;
        const std::vector<Component*>& components = entity.getComponents();
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            GLFWWindow* window = (*it)->asType<GLFWWindow>();
            if (window) {
                window->makeCurrent();
            }
        }
    }
};

class GLFWPollEvents : public sandbox::Task {
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
            ec->tasks().addType<GLFWSwapBuffers>("GLFWSwapBuffers");
            ec->tasks().addType<GLFWPollEvents>("GLFWPollEvents");
            ec->tasks().addType<GLFWMakeCurrent>("GLFWMakeCurrent");
        }
	}
}
