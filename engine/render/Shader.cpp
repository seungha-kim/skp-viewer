#include "Shader.h"
#include "checkError.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace acon {

static std::string readResourceAsStr(std::string_view name) {
    static const std::string resourceBasePath = "resources/";
    std::ifstream ifs;
    std::ostringstream oss;

    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(resourceBasePath + name.data());
    oss << ifs.rdbuf();
    return oss.str();
}

void replacePlaceholder(std::string& code, std::string_view defs) {
    static std::string placeholderStr = "//PLACEHOLDER//";
    auto pos = code.find(placeholderStr);
    if (pos != std::string::npos) {
        code.replace(pos, placeholderStr.size(), defs);
    }
}

Shader::Shader(const char* vShaderName, const char* fShaderName, const char* vShaderDefs, const char* fShaderDefs) {
    std::string resourceBasePath = "resources/";
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // TODO
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vertexCode = readResourceAsStr(vShaderName);
        fragmentCode = readResourceAsStr(fShaderName);
    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    replacePlaceholder(vertexCode, vShaderDefs);
    replacePlaceholder(fragmentCode, fShaderDefs);
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    // print linking errors if any
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(m_ID);
}

void Shader::use() {
    glUseProgram(m_ID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::set4f(const std::string& name, float v0, float v1, float v2, float v3) {
    glUniform4f(glGetUniformLocation(m_ID, name.c_str()), v0, v1, v2, v3);
}

void Shader::setMatrix4f(const std::string& name, glm::mat4 m) {
    unsigned loc = glGetUniformLocation(m_ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setVector3f(const std::string& name, glm::vec3 v) {
    unsigned loc = glGetUniformLocation(m_ID, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(v));
}

void Shader::setVector4fv(const std::string& name, glm::vec4* v, int count) {
    unsigned loc = glGetUniformLocation(m_ID, name.c_str());
    glUniform4fv(loc, count, glm::value_ptr(v[0]));
}

void Shader::setMatrix3f(const std::string& name, glm::mat3 m) {
    unsigned loc = glGetUniformLocation(m_ID, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

}
