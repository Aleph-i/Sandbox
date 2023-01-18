#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"
#include "picojson.h"
#include <fstream>

void parseJsonFile(picojson::value& val, const std::string& filePath) {
    std::ifstream file(filePath);

    std::string err = picojson::parse(val, file);
    if (!err.empty()) {
        std::cerr << "Parse error: " << err << std::endl;
    }
}

std::string getDirectory(const std::string& filePath) {
    const size_t last_slash_idx = filePath.rfind('/');
    if (std::string::npos != last_slash_idx) {
        return filePath.substr(0, last_slash_idx);
    }
    return "";
}

class PicoJsonFile : public sandbox::Component {
public:
    PicoJsonFile(const sandbox::ParameterSet& params) : loaded(false) {
        addType<PicoJsonFile>();
        addAttribute(new sandbox::TypedAttributeRef<std::string>("filePath", filePath));
    }

    ~PicoJsonFile() {}

    void update() {
        if (!loaded) {
            std::cout << "parse from: " << filePath << std::endl;
            parseJsonFile(val, filePath);

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
    PicoJsonParsedTree(const sandbox::ParameterSet& params, sandbox::EntityComponentInterface* ec) : loaded(false), ec(ec), parentEntity(NULL) {
        addType<PicoJsonParsedTree>();
        addAttribute(new sandbox::TypedAttributeRef<sandbox::Entity*>("entity", parentEntity));
    }

    ~PicoJsonParsedTree() {}

    void addEntity(sandbox::Entity& parent, picojson::object& obj, const std::string directory) {
        picojson::object::iterator it = obj.find("include");
        if (it != obj.end()) {
            picojson::value val;
            std::string filePath = directory + "/" + it->second.get<std::string>();
            parseJsonFile(val, filePath);
            addFile(val, &parent, getDirectory(filePath));
            return;
        }
        
        std::string name = obj["name"].get<std::string>();
        sandbox::Entity& entity = parent.addChild(new sandbox::Entity(name));

        picojson::array& components = obj["components"].get<picojson::array>();
        for (int i = 0; i < components.size(); i++) {
            addComponent(entity, components[i].get<picojson::object>(), directory);
        }

        picojson::array& children = obj["children"].get<picojson::array>();
        for (int i = 0; i < children.size(); i++) {
            addEntity(entity, children[i].get<picojson::object>(), directory);
        }
    }

    void addComponent(sandbox::Entity& entity, picojson::object& obj, const std::string directory) {
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
                    std::string s = it->second.get<std::string>();
                    int index = s.find("${directory}");
                    std::cout << s << " " << index << std::endl;
                    if (index >= 0) {
                        s = s.replace(index, sizeof("${directory}") - 1, directory);
                        std::cout << s << std::endl;
                    }
                    component[it->first].set<std::string>(s);
                }
            }
        }
    }

    void addFile(picojson::value& val, sandbox::Entity* parent, const std::string directory) {
        if (val.is<picojson::object>()) {
            picojson::object& obj = val.get<picojson::object>();
            addEntity(*parent, obj, directory);
        }
        else if (val.is<picojson::array>()) {
            picojson::array& arr = val.get<picojson::array>();
            for (int i = 0; i < arr.size(); i++) {
                addEntity(*parent, arr[i].get<picojson::object>(), directory);
            }
        }
    }

    void update() {
        if (!loaded) {
            sandbox::Entity* parent = getEntity();
            if (parentEntity) {
                parent = parentEntity;
            }

            PicoJsonFile* file = getEntity()->getComponent<PicoJsonFile>();
            if (file) {
                std::string filePath = (*file)["filePath"].get<std::string>();

                file->update();
                picojson::value& val = file->getValue();

                addFile(val, parent, getDirectory(filePath));
            }

            loaded = true;

            parent->update();
        }
    }

private:
    std::string filePath;
    picojson::value val;
    bool loaded;
    sandbox::EntityComponentInterface* ec;
    sandbox::Entity* parentEntity;
};

extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<PicoJsonFile>("PicoJsonFile");
            ec->components().addTypeWithParam<PicoJsonParsedTree, sandbox::EntityComponentInterface*>("PicoJsonParsedTree", ec);
        }
	}
}