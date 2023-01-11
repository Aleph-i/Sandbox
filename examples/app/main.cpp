#include <iostream>

#include "sandbox/plugin/plugin_manager.h"
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox/entity.h"
#include <unistd.h>
#include "sandbox_threading/async_update.h"
#include "sandbox_geometry/mesh.h"

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

sandbox::Entity& createShader(sandbox::Entity& entity, const std::string& name, const std::string& vshPath, const std::string& fshPath, sandbox::EntityComponentInterface* ec) {
    using namespace sandbox;

    Entity& shader = entity.addChild(new Entity(name));
        shader.addComponent(ec->components().create("OpenGLShaderProgram"));
        Component& vsh = shader.addComponent(ec->components().create("OpenGLShader"));
            vsh["shaderType"].set<std::string>("vertex"); 
            vsh["filePath"].set<std::string>(vshPath);
        Component& fsh = shader.addComponent(ec->components().create("OpenGLShader"));
            fsh["filePath"].set<std::string>(fshPath);
            fsh["shaderType"].set<std::string>("fragment");

    return shader;
}

sandbox::Entity& createMesh(sandbox::Entity& entity, const std::string& name, const std::string& filePath, sandbox::EntityComponentInterface* ec) {
    using namespace sandbox;

    Entity& obj = entity.addChild(new Entity(name));
        Component& mesh = obj.addComponent(ec->components().create("AssimpMeshLoader"));
            mesh["filePath"].set<std::string>("../examples/app/data/models/monkey-head.obj");

    return obj;
}


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

        Entity& geometry = root.addChild(new Entity("Geometry"));
            Entity& object = createMesh(geometry, "Object", "../examples/app/data/models/monkey-head.obj", ec);
            Entity& triangle = geometry.addChild(new Entity("Triangle"));
                Component& tri = triangle.addComponent(ec->components().create("SimpleTriangle"));

        Entity& scene = root.addChild(new Entity("Scene"));
            Entity& camera = scene.addChild(new Entity("Camera"));
                Component& cam = camera.addComponent(ec->components().create("GlmCamera"));

        Entity& display = root.addChild(new Entity("Display"));
            Component& window = display.addComponent(ec->components().create("GLFWWindow"))
                .setAttribute<int>("width", 700);
            Entity& shaders = display.addChild(new Entity("Shaders"));
                Entity& simple = createShader(shaders, "Simple", "../examples/app/data/shaders/simple.vsh", "../examples/app/data/shaders/simple.fsh", ec);
                Entity& complex = createShader(shaders, "Complex", "../examples/app/data/shaders/simple.vsh", "../examples/app/data/shaders/simple.fsh", ec);
            Entity& commands = display.addChild(new Entity("Commands"));
                commands.addComponent(new ComponentProxy(&cam));
                Component& shaderCommand = commands.addComponent(ec->components().create("OpenGLShaderCommand"))
                    .setAttribute<Entity*>("shaderProgram", &simple);
                Entity& triangle2 = commands.addChild(new Entity("Triangle"));
                    triangle2.addComponent(ec->components().create("OpenGLTest"))
                        .setAttribute<Entity*>("mesh", &triangle);

    
    root.update();

    std::vector<float>& pos = object.getChildren()[0]->getComponents()[0]->asType<sandbox::Mesh>()->getFloatArray("a");
    std::cout << "pos " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;

    Task& print = *ec->tasks().create("PrintTask");
    root.runTask(print);

    Task& pollEvents = *ec->tasks().create("GLFWPollEvents");
    Task& render = *ec->tasks().create("GLFWRender");

    int count = 0;

    while (true) {
        //usleep(100000);
        display.runTask(render);
        //display2.runTask(render2);
        root.runTask(pollEvents);

        /*if (count == 60) {
            shaders.deleteChild(&simple);
            commands.deleteComponent(&shaderCommand);
        }*/
            //std::cout << count << std::endl;

        count++;
    }

    return 0;
}