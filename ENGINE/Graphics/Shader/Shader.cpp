//
// Created by Anhelina Modenko on 30.06.2023.
//

#include "Shader.h"

Shader::Shader() {}

Shader::Shader(bool includeDefaultDirectory,
                     const char* vertexShaderPath, const char* fragmentShaderPath,
                     const char* geometryShaderPath) {
    generate(includeDefaultDirectory, vertexShaderPath, fragmentShaderPath, geometryShaderPath);
}

void Shader::generate(bool includeDefaultDirectory,
                      const char* vertexShaderPath, const char* fragmentShaderPath,
                      const char* geometryShaderPath) {
    int success;
    char infoLog[512];

    GLuint vertexShader = compileShader(includeDefaultDirectory, vertexShaderPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(includeDefaultDirectory, fragmentShaderPath, GL_FRAGMENT_SHADER);

    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << vertexShaderPath << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

void Shader::activate() {
    glUseProgram(id);
}

void Shader::cleanup() {
    glDeleteProgram(id);
}

std::string Shader::loadShaderSrc(bool includeDefaultDirectory, const char *filepath) {
    if (includeDefaultDirectory)
        std::string fullPath = Shader::defaultDirectory + '/' + filepath;

    std::ifstream file;
    std::stringstream buf;

    std::string ret = "";

    file.open(filepath);

    if (file.is_open()) {
        buf << file.rdbuf();
        ret = buf.str();
    } else {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << filepath << std::endl;
    }

    file.close();

    return ret;
}

GLuint Shader::compileShader(bool includeDefaultDirectory, const char *filepath, GLenum type) {
    int success;
    char infoLog[512];

    GLuint ret = glCreateShader(type);
    std::string shaderSrc = loadShaderSrc(includeDefaultDirectory, filepath);
    const GLchar* shader = shaderSrc.c_str();

    glShaderSource(ret, 1, &shader, nullptr);
    glCompileShader(ret);

    glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ret, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return ret;
}

void Shader::setMat4(const std::string &name, glm::mat4 val) {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setBool(const std::string& name, bool value) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set3Float(const std::string& name, float v1, float v2, float v3) {
    glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set3Float(const std::string& name, glm::vec3 v) {
    glUniform3f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z);
}

void Shader::set4Float(const std::string& name, float v1, float v2, float v3, float v4) {
    glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
}

void Shader::set4Float(const std::string& name, aiColor4D color) {
    glUniform4f(glGetUniformLocation(id, name.c_str()), color.r, color.g, color.b, color.a);
}

void Shader::set4Float(const std::string& name, glm::vec4 v) {
    glUniform4f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::setMat3(const std::string& name, glm::mat3 val) {
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

std::stringstream Shader::defaultHeader;

void Shader::clearDefault() {
    Shader::defaultHeader.clear();
}

void Shader::loadIntoDefault(const char* filepath) {
    std::basic_string<char> fileContents = loadShaderSrc(false, filepath);

    Shader::defaultHeader << fileContents;

    fileContents = "";
}