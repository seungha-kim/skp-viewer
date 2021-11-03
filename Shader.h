//
// Created by Kim Seungha on 2021/11/03.
//

#ifndef LEARN_OPENGL_SHADER_H
#define LEARN_OPENGL_SHADER_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader {
public:
    // the program ID
    unsigned int m_ID;

    // constructor reads and builds the shader
    Shader(const char *vShaderName, const char *fShaderName);
    ~Shader();
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void set4f(const std::string& name, float v0, float v1, float v2, float v3);
    void setMatrix4f(const std::string& name, glm::mat4 m);
};


#endif //LEARN_OPENGL_SHADER_H
