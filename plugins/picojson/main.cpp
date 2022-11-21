#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"
#include "picojson.h"
#include <fstream>

class PicoJsonFile : public sandbox::Component {
public:
    PicoJsonFile() : loaded(false) {
        addType<PicoJsonFile>();
        addAttribute(new sandbox::TypedAttributeRef<std::string>("filePath", filePath));
    }

    ~PicoJsonFile() {}

    void update() {
        if (!loaded) {
            std::cout << "parse from: " << filePath << std::endl;
            std::ifstream file(filePath);

            std::string err = picojson::parse(val, file);
            if (!err.empty()) {
                std::cerr << "Parse error: " << err << std::endl;
            }


            std::cout << "parsed value: " << std::endl;
            std::cout << val.serialize() << std::endl;

            loaded = true;
        }
    }

    picojson::value& getValue() { return val; } 

private:
    std::string filePath;
    picojson::value val;
    bool loaded;
};

class PicoJsonParsedTree : public sandbox::Component {
public:
    PicoJsonParsedTree(sandbox::EntityComponentInterface* ec) : loaded(false), ec(ec) {
        addType<PicoJsonParsedTree>();
    }

    ~PicoJsonParsedTree() {}

    void addEntity(sandbox::Entity& parent, picojson::object& obj) {
        std::string name = obj["name"].get<std::string>();
        sandbox::Entity& entity = parent.addChild(new sandbox::Entity(name));

        picojson::array& components = obj["components"].get<picojson::array>();
        for (int i = 0; i < components.size(); i++) {
            addComponent(entity, components[i].get<picojson::object>());
        }

        picojson::array& children = obj["children"].get<picojson::array>();
        for (int i = 0; i < children.size(); i++) {
            addEntity(entity, children[i].get<picojson::object>());
        }
    }

    void addComponent(sandbox::Entity& entity, picojson::object& obj) {
        std::string type = obj["type"].get<std::string>();
        sandbox::Component& component = entity.addComponent(ec->components().create(type));

        for (picojson::object::iterator it = obj.begin(); it != obj.end(); it++) {
            if (it->first != "type") {
                if (it->second.is<double>()) {
                    double val = it->second.get<double>();
                    if (component[it->first].isType<int>()) {
                        component[it->first].set<int>(val);
                    }
                }
                else if (it->second.is<std::string>()) {
                    component[it->first].set<std::string>(it->second.get<std::string>());
                }
            }
        }
    }

    void update() {
        if (!loaded) {
            PicoJsonFile* file = getEntity()->getComponent<PicoJsonFile>();
            if (file) {
                file->update();
                picojson::value& val = file->getValue();

                if (val.is<picojson::object>()) {
                    picojson::object& obj = val.get<picojson::object>();
                    addEntity(*getEntity(), obj);
                }
                else if (val.is<picojson::array>()) {
                    picojson::array& arr = val.get<picojson::array>();
                    for (int i = 0; i < arr.size(); i++) {
                        addEntity(*getEntity(), arr[i].get<picojson::object>());
                    }
                }

            }

            loaded = true;
        }
    }

private:
    std::string filePath;
    picojson::value val;
    bool loaded;
    sandbox::EntityComponentInterface* ec;
};

extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<PicoJsonFile>("PicoJsonFile");
            ec->components().addFactory(new sandbox::TypedObjectFactoryWithParam<PicoJsonParsedTree, Component, EntityComponentInterface*>("PicoJsonParsedTree", ec));
        }
	}
}