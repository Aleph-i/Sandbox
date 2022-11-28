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

namespace sandbox {

class ComponentWithCallbacks : public sandbox::Component {
public:
    ComponentWithCallbacks() : loaded(false) {}
    virtual ~ComponentWithCallbacks() {}

    void update() {
        if (!loaded) {
            callbacks = getEntity()->getComponents<sandbox::CallbackComponent>();

            loaded = true;
        }
    }

    void run(sandbox::Object& data, sandbox::Object& returnValue) {
        for (int i = 0; i < callbacks.size(); i++) {
            callbacks[i]->run(data, returnValue);
        }
    }

private:
    bool loaded;
    std::string command;
    std::vector<sandbox::CallbackComponent*> callbacks;
};

}

class GLFWCursorPosition : public sandbox::ComponentWithCallbacks {
public:
    GLFWCursorPosition() : loaded(false){
        addType<GLFWCursorPosition>();
        obj.mapValue("x", x);
        obj.mapValue("y", y);
    }

    void update();

    void updatePos(double x, double y) {
        this->x = x;
        this->y = y;
        run(obj, obj);
    }

private:
    bool loaded;
    double x;
    double y;
    sandbox::MapObject<double> obj;
};

class GLFWMouseButton : public sandbox::ComponentWithCallbacks {
public:
    GLFWMouseButton() : loaded(false){
        addType<GLFWMouseButton>();
        obj.mapValue("button", button);
        obj.mapValue("action", action);
        obj.mapValue("mods", mods);
    }

    void update();

    void updateButton(int button, int action, int mods) {
        this->button = button;
        this->action = action;
        this->mods = mods;
        run(obj, obj);
    }

private:
    bool loaded;
    int button, action, mods;
    sandbox::MapObject<int> obj;
};

class GLFWWindow : public sandbox::Component {
public:
    GLFWWindow() : window(NULL), loaded(false) {
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

    void update() {
        if (!loaded) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            
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
            int version = gladLoadGL(glfwGetProcAddress);
            printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

            glfwSwapBuffers(window);
            glfwSwapInterval(1);

            loaded = true;
        }
    }

    void swapBuffers() {
        glfwSwapBuffers(window);
    }

    void makeCurrent() {
        glfwMakeContextCurrent(window);
    }

    GLFWwindow* getWindow() { return window; }

    void setCursorPosition(GLFWCursorPosition* cursorPosition) { this->cursorPosition = cursorPosition; }
    void setMouseButton(GLFWMouseButton* mouseButton) { this->mouseButton = mouseButton; }

private:

    void sizeCallback(GLFWwindow* window, int width, int height) {
        std::cout << "sizeCallback" << std::endl;
  		this->width = width;
  		this->height = height;
  	}

    void cursorPositionCallback(GLFWwindow* window, float xpos, float ypos) {
    	std::cout << xpos/width << ", " << 1.0-ypos/height << std::endl;
        if (cursorPosition) {
            cursorPosition->updatePos(xpos/width, 1.0-ypos/height);
        }
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        //std::cout << button << ", " << action << ", " << mods << std::endl;
        mouseButton->updateButton(button, action, mods);
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
    bool loaded;
    GLFWCursorPosition* cursorPosition;
    GLFWMouseButton* mouseButton;
};


void GLFWCursorPosition::update() {
    if (!loaded) {
        GLFWWindow* windowComponent = getEntity()->getComponentFromAbove<GLFWWindow>();
        windowComponent->setCursorPosition(this);
    }

    ComponentWithCallbacks::update();
}

void GLFWMouseButton::update() {
    if (!loaded) {
        GLFWWindow* windowComponent = getEntity()->getComponentFromAbove<GLFWWindow>();
        windowComponent->setMouseButton(this);
    }

    ComponentWithCallbacks::update();
}


/*class GLFWInitContext : public sandbox::TypedRecursiveTask<GLFWWindow> {
public:
    void runComponent(GLFWWindow& window, sandbox::TaskContext* context) {
        window.init();
    }
};*/

class GLFWSwapBuffers : public sandbox::TypedRecursiveTask<GLFWWindow> {
public:
    void runComponent(GLFWWindow& window, sandbox::TaskContext* context) {
        window.swapBuffers();
    }
};

class GLFWMakeCurrent : public sandbox::TypedRecursiveTask<GLFWWindow> {
public:
    void runComponent(GLFWWindow& window, sandbox::TaskContext* context) {
        window.makeCurrent();
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
            ec->components().addType<GLFWCursorPosition>("GLFWCursorPosition");
            ec->components().addType<GLFWMouseButton>("GLFWMouseButton");
            //ec->tasks().addType<GLFWInitContext>("GLFWInitContext");
            ec->tasks().addType<GLFWSwapBuffers>("GLFWSwapBuffers");
            ec->tasks().addType<GLFWPollEvents>("GLFWPollEvents");
            ec->tasks().addType<GLFWMakeCurrent>("GLFWMakeCurrent");
        }
	}
}
