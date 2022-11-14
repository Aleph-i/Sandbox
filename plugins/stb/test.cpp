#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


class STBImage : public sandbox::Component {
public:
    STBImage() {
        addType<STBImage>();
        std::string path;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &components, STBI_rgb_alpha);
    }

private:
    int width, height, components;
};


extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<STBImage>("STBImage");
        }
	}
}