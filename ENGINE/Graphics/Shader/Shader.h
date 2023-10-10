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

    Shader(bool includeDefaultHeader,
           const char* vertexShaderPath,
           const char* fragShaderPath,
           const char* geoShaderPath = nullptr);

    void generate(bool includeDefaultHeader,
                  const char* vertexShaderPath,
                  const char* fragShaderPath,
                  const char* geoShaderPath = nullptr);

    void activate();

    void cleanup();

    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void set3Float(const std::string& name, float v1, float v2, float v3);
    void set3Float(const std::string& name, glm::vec3 v);
    void set4Float(const std::string& name, float v1, float v2, float v3, float v4);
    void set4Float(const std::string& name, aiColor4D color);
    void set4Float(const std::string& name, glm::vec4 v);
    void setMat3(const std::string& name, glm::mat3 val);
    void setMat4(const std::string& name, glm::mat4 val);

    static GLuint compileShader(bool includeDefaultHeader, const char* filePath, GLuint type);
    static std::string defaultDirectory;
    static std::stringstream defaultHeaders;
    static void loadIntoDefault(const char* filepath);
    static void clearDefault();
    static char *loadShaderSrc(bool includeDefaultHeader, const char* filePath);
};


#endif //MARCHING_CUBES_SHADER_H
