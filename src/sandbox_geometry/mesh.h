#ifndef SANDBOX_GEOMETRY_MESH_H_
#define SANDBOX_GEOMETRY_MESH_H_

#include <map>
#include <vector>

namespace sandbox {

class Mesh {
public:
    virtual ~Mesh() {}

    virtual std::vector<float>& getFloatArray(const std::string& name) = 0;
    virtual std::vector<int>& getIntArray(const std::string& name) = 0;
};

}

#endif