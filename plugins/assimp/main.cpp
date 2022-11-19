#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AssimpMesh : public sandbox::Component {
public:
    AssimpMesh() : loaded(false) {
        addType<AssimpMesh>();
        addAttribute(new sandbox::TypedAttributeRef<std::string>("filePath", filePath));
    }

    void update() {
        if (!loaded) {
            loaded = true;

            Assimp::Importer importer;
			const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals); // aiProcess_GenNormals);//

			if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			}

			std::cout << "Num meshes: " << scene->mNumMeshes << std::endl;
        }
    }

private:
    std::string filePath;
    bool loaded;
};


extern "C"
{
	void registerInterface(sandbox::PluginInterface* interface) {
        using namespace sandbox;
        EntityComponentInterface* ec = dynamic_cast<EntityComponentInterface*>(interface);
        if (ec) {
            ec->components().addType<AssimpMesh>("AssimpMesh");
        }
	}
}