#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


class STBImageRGBA : public sandbox::Component {
public:
    STBImageRGBA() : loaded(false) {
        addType<STBImageRGBA>();
        addAttribute(new sandbox::TypedAttributeRef<std::string>("filePath", filePath));
        addAttribute(new sandbox::TypedAttributeRef<int>("width", width));
        addAttribute(new sandbox::TypedAttributeRef<int>("height", height));
        addAttribute(new sandbox::TypedAttributeRef<int>("components", components));
    }

    void update() {
        if (!loaded) {
            unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &components, STBI_rgb_alpha);
            components = 4;

            std::cout << width << " " << height << std::endl;

            loaded = true;
        }
    }

private:
    std::string filePath;
    int width, height, components;
    bool loaded;
};


extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<STBImageRGBA>("STBImageRGBA");
        }
	}
}