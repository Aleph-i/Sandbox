#include <iostream>

#include "sandbox/plugin/plugin_manager.h"
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox/entity.h"
#include <unistd.h>

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

    Entity root("Root");
        Entity& display = root.addChild(new Entity("Display"));
            Component& window = display.addComponent(ec->components().create("GLFWWindow"));
                window["width"].set<int>(700);
            Entity& triangle = display.addChild(new Entity("Triangle"));
                triangle.addComponent(ec->components().create("OpenGLTest"));
        Entity& geometry = root.addChild(new Entity("Geometry"));
            Entity& object = geometry.addChild(new Entity("Object"));
                Component& mesh = object.addComponent(ec->components().create("AssimpMesh"));
                mesh["filePath"].set<std::string>("../examples/app/data/models/monkey-head.obj");
        Entity& images = root.addChild(new Entity("Images"));
            Component& image = images.addComponent(ec->components().create("STBImageRGBA"));
            image["filePath"].set<std::string>("../examples/app/data/images/img_test.png");
        Entity& webServer = root.addChild(new Entity("WebServer"));
            Component ws = webServer.addComponent(ec->components().create("CppWebServer"));
            ws["port"].set<int>(8081);
            ws["webDir"].set<std::string>("../examples/app/data/web");
            Component ws2 = webServer.addComponent(ec->components().create("CppWebServer"));
            ws2["port"].set<int>(8082);
            ws2["webDir"].set<std::string>("../examples/app/data/web");
    
    root.update();

    Task& print = *ec->tasks().create("PrintTask");
    root.runTask(print);

    //Task& initContext = *ec->tasks().create("GLFWInitContext");
    //root.runTask(initContext);

    Task& swapBuffers = *ec->tasks().create("GLFWSwapBuffers");
    Task& pollEvents = *ec->tasks().create("GLFWPollEvents");
    Task& makeCurrent = *ec->tasks().create("GLFWMakeCurrent");
    Task& init = *ec->tasks().create("OpenGLInit");
    Task& run = *ec->tasks().create("OpenGLRun");

    root.runTask(makeCurrent);
    root.runTask(init);
    root.runTask(run);
    root.runTask(swapBuffers);
    root.runTask(pollEvents);

    while (true) {
        usleep(100000);

        root.runTask(makeCurrent);
        root.runTask(run);
        root.runTask(swapBuffers);
        root.runTask(pollEvents);
        //webServer.update();

        std::cout << window["width"].get<int>() << std::endl;
        //std::cout << "step" << std::endl;
    }

    return 0;
}