#include <iostream>

#include "sandbox/plugin/plugin_manager.h"
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox/entity.h"
#include <unistd.h>
#include "sandbox_threading/async_update.h"

class PrintTest : public sandbox::Component {
public:
    PrintTest() {
        addType<PrintTest>();
    }

    void update() {
        std::cout << "PrintTest" << std::endl;
    }
};


class UpdateCommand : public sandbox::CallbackComponent {
public:
    UpdateCommand(std::mutex& mutex) : mutex(mutex) {}
    void run(sandbox::Object& data, sandbox::Object& returnValue) {
        std::unique_lock<std::mutex> lock(mutex);
        std::cout << "Something: " << data["id"].get<double>() << std::endl;
    }

private:
    std::mutex& mutex;
};


class CursorPosition : public sandbox::CallbackComponent {
public:
    void run(sandbox::Object& data, sandbox::Object& returnValue) {
        std::cout << "Something: " << data["x"].get<double>() << " " << data["y"].get<double>() << std::endl;
    }
};

class MouseButton : public sandbox::CallbackComponent {
public:
    void run(sandbox::Object& data, sandbox::Object& returnValue) {
        std::cout << "Something: " << data["button"].get<int>() << " " << data["action"].get<int>() << " " << data["mods"].get<int>() << std::endl;
    }
};


int main(int argc, char *argv[]) {
    std::cout << "This is a test." << std::endl;

    using namespace sandbox;

    PluginManager pm;
    EntityComponentInterface* ec = new EntityComponentInterface();
    pm.addPluginInterface(ec);
    pm.loadPlugin("lib/libtest.so");
    pm.loadPlugin("lib/libglfw_sandbox.so");
    pm.loadPlugin("lib/libopengl_sandbox.so");
    pm.loadPlugin("lib/libstb_sandbox.so");
    pm.loadPlugin("lib/libassimp_sandbox.so");
    pm.loadPlugin("lib/libcppws_sandbox.so");
    pm.loadPlugin("lib/libpicojson_sandbox.so");
    pm.loadPlugin("lib/libglm_sandbox.so");
    ec->components().addType<sandbox::AsyncUpdate>("AsyncUpdate");

    std::mutex updateMutex;

    Entity root("Root");
        Entity& scene = root.addChild(new Entity("Scene"));
            Entity& camera = scene.addChild(new Entity("Camera"));
                Component& cam = camera.addComponent(ec->components().create("GlmCamera"));
        Entity& display = root.addChild(new Entity("Display"));
            Component& window = display.addComponent(ec->components().create("GLFWWindow"));
                window["width"].set<int>(700);
            Entity& shaders = display.addChild(new Entity("Shaders"));
                Entity& simple = shaders.addChild(new Entity("Simple"));
                    simple.addComponent(ec->components().create("OpenGLShaderProgram"));
                    Component& vsh = simple.addComponent(ec->components().create("OpenGLShader"));
                        vsh["shaderType"].set<std::string>("vertex"); 
                        vsh["filePath"].set<std::string>("../examples/app/data/shaders/simple.vsh");
                    Component& fsh = simple.addComponent(ec->components().create("OpenGLShader"));
                        fsh["filePath"].set<std::string>("../examples/app/data/shaders/simple.fsh");
                        fsh["shaderType"].set<std::string>("fragment");
            Entity& commands = display.addChild(new Entity("Commands"));
                Component& shaderCommand = commands.addComponent(ec->components().create("OpenGLShaderCommand"));
                        shaderCommand["shader"].set<Entity*>(&simple);
                Entity& triangle = commands.addChild(new Entity("Triangle"));
                    triangle.addComponent(new ComponentProxy(&cam));
                    //triangle.addComponent(ec->components().create("GlmCamera"));
                    triangle.addComponent(ec->components().create("OpenGLTest"));
        /*Entity& display2 = root.addChild(new Entity("Display"));
            Component& window2 = display2.addComponent(ec->components().create("GLFWWindow"));
                window2["width"].set<int>(700);
            Entity& triangle2 = display2.addChild(new Entity("Triangle"));
                triangle2.addComponent(new ComponentProxy(&cam));
                //triangle2.addComponent(ec->components().create("GlmCamera"));
                Component& t2 = triangle2.addComponent(ec->components().create("OpenGLTest"));
                t2["dir"].set<bool>(false);*/
        Entity& geometry = root.addChild(new Entity("Geometry"));
            Entity& object = geometry.addChild(new Entity("Object"));
                Component& mesh = object.addComponent(ec->components().create("AssimpMesh"));
                mesh["filePath"].set<std::string>("../examples/app/data/models/monkey-head.obj");
    
    root.update();


    Task& print = *ec->tasks().create("PrintTask");
    root.runTask(print);

    Task& pollEvents = *ec->tasks().create("GLFWPollEvents");
    Task& render = *ec->tasks().create("GLFWRender");
    Task& render2 = *ec->tasks().create("GLFWRender");

    while (true) {
        //usleep(100000);
        display.runTask(render);
        //display2.runTask(render2);
        root.runTask(pollEvents);
    }

    return 0;
}