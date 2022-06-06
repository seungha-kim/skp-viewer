#pragma once

#include "UniformBinding.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

namespace acon {

class Shader {
public:
    // the program ID
    unsigned int m_ID;

    // constructor reads and builds the shader
    Shader(
        const char* vShaderName,
        const char* fShaderName,
        const char* vShaderDefs = "",
        const char* fShaderDefs = "",
        const std::vector<UniformBinding>& bindings = {});
    ~Shader();

    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void set4f(const std::string& name, float v0, float v1, float v2, float v3);
    void setMatrix4f(const std::string& name, glm::mat4 m);
    void setMatrix3f(const std::string& name, glm::mat3 m);
    void setVector3f(const std::string& name, glm::vec3 v);
    void setVector4fv(const std::string& name, glm::vec4* vs, int count);
};

}
