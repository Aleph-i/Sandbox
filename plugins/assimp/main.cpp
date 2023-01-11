#include <iostream>
#include "sandbox/interfaces/entity_component_interface.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <sandbox_geometry/mesh.h>

class AssimpMesh : public sandbox::Mesh {
public:
    AssimpMesh() {
        std::vector<float> a;
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        floatArrays.push_back(a);
    }

    std::vector<float>& getFloatArray(const std::string& name) {
        return floatArrays[0];
    }

    std::vector<int>& getIntArray(const std::string& name) {
        return intArrays[0];
    }

private:
    std::vector< std::vector<float> > floatArrays;
    std::vector< std::vector<int> > intArrays;
};

class AssimpMeshLoader : public sandbox::Component {
public:
    AssimpMeshLoader() : loaded(false) {
        addType<AssimpMeshLoader>();
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

            /*std::vector<float> a;
            a.push_back(1);
            a.push_back(2);
            a.push_back(3);
            //floatArrays.push_back(a);
            addAttribute(new sandbox::TypedAttributeRef<std::vector<float>>("a", floatArrays[0]));*/

            sandbox::Entity& mesh = getEntity()->addChild(new sandbox::Entity("Mesh"));
            mesh.addComponent(new sandbox::ComponentWrapper<AssimpMesh, sandbox::Mesh>());

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
            ec->components().addType<AssimpMeshLoader>("AssimpMeshLoader");
        }
	}
}