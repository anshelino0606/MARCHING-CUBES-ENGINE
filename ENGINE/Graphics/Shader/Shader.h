//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_SHADER_H
#define MARCHING_CUBES_SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:

    unsigned int id;
    Shader();
    Shader(bool includeDefaultDirectory,
           const char* vertexShaderPath, const char* fragmentShaderPath,
           const char* geometryShaderPath = nullptr);

    void generate(bool includeDefaultDirectory,
                const char* vertexShaderPath, const char* fragmentShaderPath,
                const char* geometryShaderPath = nullptr);

    void activate();
    void cleanup();

    // utility
    static std::string loadShaderSrc(bool includeDefaultDirectory, const char* filepath);
    GLuint compileShader(bool includeDefaultDirectory, const char* filepath, GLenum type);
    static void loadIntoDefault(const char* filepath);
    static void clearDefault();


    // uniform
    void setMat4(const std::string &name, glm::mat4 value);
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void set3Float(const std::string& name, float v1, float v2, float v3);
    void set3Float(const std::string& name, glm::vec3 v);
    void set4Float(const std::string& name, float v1, float v2, float v3, float v4);
    void set4Float(const std::string& name, aiColor4D color);
    void set4Float(const std::string& name, glm::vec4 v);
    void setMat3(const std::string& name, glm::mat3 val);

    static std::string defaultDirectory;
    static std::stringstream defaultHeader;
};


#endif //MARCHING_CUBES_SHADER_H
