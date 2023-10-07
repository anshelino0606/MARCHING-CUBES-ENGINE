//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_SCENE_H
#define MARCHING_CUBES_SCENE_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <map>

#include <glm/glm.hpp>

#include <json/json.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ENGINE/Graphics/Memory/FrameMemory.h"
#include "ENGINE/Graphics/Memory/VertexMemory.h"

#include "ENGINE/Models/box.hpp"
#include "ENGINE/Graphics/Model/Model.h"
#include "ENGINE/Graphics/Light/Light.h"
#include "ENGINE/Graphics/Shader/Shader.h"
#include "ENGINE/IO/Camera.h"
#include "ENGINE/IO/Keyboard.h"
#include "ENGINE/IO/Mouse.h"
#include "ENGINE/Algorithms/States/States.h"
#include "ENGINE/Algorithms/AVL/Avl.h"
#include "ENGINE/Algorithms/Octree/Octree.h"
#include "ENGINE/Algorithms/Trie/Trie.h"
#include "ENGINE/Graphics/Text/TextRenderer.h"

namespace Octree {
    class node;
}

class Model;


class Scene {
public:
    Scene();

    Scene(int glfwVersionMajor, int glfwVersionMinor,
          const char* title, unsigned int scrWidth, unsigned int scrHeight);

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    bool init();
    bool registerFont(TextRenderer* tr, std::string name, std::string path);

    void prepare(Box &box, std::vector<Shader> shaders);

    void processInput(float dt);
    void update();
    void newFrame(Box &box);

    void renderShader(Shader shader, bool applyLighting = true);
    void renderDirLightShader(Shader shader);
    void renderPointLightShader(Shader shader, unsigned int idx);
    void renderSpotLightShader(Shader shader, unsigned int idx);
    void renderInstances(std::string modelId, Shader shader, float dt);

    FT_Library ft;
    AVL* fonts;

    void renderText(std::string font, Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color);

    void cleanup();
    bool shouldClose();
    Camera* getActiveCamera();

    void setShouldClose(bool shouldClose);
    void setWindowColor(float r, float g, float b, float a);

    void registerModel(Model* model);
    RigidBody* generateInstance(std::string modelId,
                                glm::vec3 size = glm::vec3(1.0f),
                                float mass = 1.0f,
                                glm::vec3 pos = glm::vec3(0.0f),
                                glm::vec3 rot = glm::vec3(0.0f));

    void initInstances();
    void loadModels();

    void removeInstance(std::string instanceId);
    void markForDeletion(std::string instanceId);
    void clearDeadInstances();

    std::string currentId;
    std::string generateId();


    unsigned int noPointLights;
    std::vector<PointLight*> pointLights;
    unsigned int activePointLights;

    unsigned int noSpotLights;
    std::vector<SpotLight*> spotLights;
    unsigned int activeSpotLights;

    DirLight* dirLight;
    bool dirLightActive;

    std::vector<Camera*> cameras;
    unsigned int activeCamera;

    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 textProjection;
    glm::vec3 cameraPos;

    AVL* models;
    trie::Trie<RigidBody*> instances;

    std::vector<RigidBody*> instancesToDelete;
    Octree::node* octree;

    Json::Value variableLog;

    FrameBuffer defaultFBO;

    UniformMemory::UBO lightUBO;

protected:
    GLFWwindow* window;

    const char* title;
    static unsigned int scrWidth;
    static unsigned int scrHeight;

    float bg[4];

    int glfwVersionMajor;
    int glfwVersionMinor;
};


#endif //MARCHING_CUBES_SCENE_H
