#include <iostream>
#include "sandbox/plugin/plugin_interface.h"
#include "sandbox/interfaces/entity_component_interface.h"

class TestComponent : public sandbox::Component {
public:
    TestComponent() {
        addType<TestComponent>();
    }
};

class ConsoleDisplay : public sandbox::Component { //, public sandbox::Renderable {
public:
    ConsoleDisplay() {
        addType<ConsoleDisplay>();
        //addType<sandbox::Renderable>(static_cast<sandbox::Renderable*>(this));
    }
    
    void Render() {
        std::cout << "Render console" << std::endl;
    }

    void update() {
        std::cout << "Update console" << std::endl;
    }
};

class PrintTask : public sandbox::RecursiveTask {
public:
    class PrintContext : public sandbox::TaskContext {
    public:
        PrintContext() {
            indent.push_back("");
        }
        void push() {
            indent.push_back(indent[indent.size()-1] + "\t");
        }
        void pop() {
            indent.pop_back();
        }
        std::string& get() {
            return indent[indent.size()-1];
        }
        std::vector<std::string> indent;
    };

    sandbox::TaskContext* createContext() { return new PrintContext(); }

    void runEntity(sandbox::Entity& entity, sandbox::TaskContext* context) {
        using namespace sandbox;
        PrintContext* printContext = static_cast<PrintContext*>(context);
        std::cout << printContext->get() << entity.getName() << std::endl;
        const std::vector<Component*>& components = entity.getComponents();
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            if (*it != NULL) {
                std::cout << printContext->get() << "\t" << *it << " (";
                for (int i = 0; i < (*it)->getTypes().size(); i++) {
                    if (i > 0) {
                        std::cout << ", ";
                    }
                    std::cout << (*it)->getTypes()[i]->name();
                }
                std::cout << ")" << std::endl;

                for (int i = 0; i < (*it)->getAttributes().size(); i++) {
                    std::cout << printContext->get() << "\t" << " - " << (*it)->getAttributes()[i]->getName() << std::endl;
                }
            }
        }
    }
};

/*class RenderTask : public sandbox::RecursiveTask {
public:
    void runEntity(sandbox::Entity& entity, sandbox::TaskContext* context) {
        using namespace sandbox;
        const std::vector<Component*>& components = entity.getComponents();
        for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); it++) {
            sandbox::Renderable* renderable = (*it)->asType<sandbox::Renderable>();
            if (renderable) {
                renderable->Render();
            }
        }
    }
};*/

extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;
		std::cout << "Plugin Loaded." << std::endl;
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<TestComponent>("TestComponent");
            ec->components().addType<ConsoleDisplay>("ConsoleDisplay");
            ec->tasks().addType<PrintTask>("PrintTask");
            //ec->tasks().addType<RenderTask>("RenderTask");
        }
	}
}