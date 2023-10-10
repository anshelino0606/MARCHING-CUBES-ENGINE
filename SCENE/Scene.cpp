#define GLFW_INCLUDE_GLCOREARB
#include "Scene.h"
#include "LIBRARIES/imgui/imgui.h"
#include "LIBRARIES/imgui/backends/imgui_impl_glfw.h"
#include "LIBRARIES/imgui/backends/imgui_impl_opengl3.h"
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 2

#define WINDOW_WIDTH 1680
#define WINDOW_HEIGHT 1020

unsigned int Scene::scrWidth = WINDOW_WIDTH*2;
unsigned int Scene::scrHeight = WINDOW_HEIGHT*2;



void Scene::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    // Calculate the content scale factor
    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);

    // Adjust the framebuffer size based on the content scale
    fbWidth *= xscale;
    fbHeight *= yscale;

    glViewport(0, 0, fbWidth, fbHeight);



    Scene::scrHeight = fbHeight;
    Scene::scrWidth = fbWidth;
}

Scene::Scene()
        : currentId("aaaaaaaa"), lightUBO(0) {}

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor,
             const char* title, unsigned int scrWidth, unsigned int scrHeight)
        : glfwVersionMajor(glfwVersionMajor), glfwVersionMinor(glfwVersionMinor), // GLFW version
          title(title), // window title
          activeCamera(-1),
          activePointLights(0), activeSpotLights(0),
          currentId("aaaaaaaa"), lightUBO(0) {

    Scene::scrWidth = scrWidth*2;
    Scene::scrHeight = scrHeight*2;
    defaultFBO = FrameBuffer(scrWidth, scrHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    setWindowColor(0.5f, 0.5f, 0.5f, 1.0f);
}

bool Scene::init() {
    glfwInit();

    // set version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // resize
//    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
//    glfwWindowHint(GLFW_RESIZABLE, false);

    // initialize window
    window = glfwCreateWindow(WINDOW_WIDTH*2, WINDOW_HEIGHT*2, title, nullptr, nullptr);
    if (window == NULL) {
        // not created
        return false;
    }
    glfwMakeContextCurrent(window);

    // set GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, Keyboard::keyCallback);
    glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
    glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
    glfwSetScrollCallback(window, Mouse::mouseWheelCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST); // doesn't show vertices not visible to camera (back of object)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    models = avl_createEmptyRoot(strkeycmp);
    instances = trie::Trie<RigidBody*>(trie::ascii_lowercase);

    octree = new Octree::node(Bounds(glm::vec3(-16.0f), glm::vec3(16.0f)));

    if (FT_Init_FreeType(&ft)) {
        std::cout << "Could not init FreeType library" << std::endl;
        return false;
    }
    fonts = avl_createEmptyRoot(strkeycmp);
    Scene::state = WindowStates::ACTIVE;

    return true;
}

bool Scene::registerFont(TextRenderer* tr, std::string name, std::string path) {
    if (tr->loadFont(ft, path)) {
        fonts = avl_insert(fonts, (void*)name.c_str(), tr);
        return true;
    }
    else {
        return false;
    }
}

void Scene::prepare(Box& box, std::vector<Shader> shaders) {
    FT_Done_FreeType(ft);
    octree->update(box);

    lightUBO = UniformMemory::UBO(0, {
            UniformMemory::newStruct({
                                   UniformMemory::Type::VEC3,

                                   UniformMemory::Type::VEC4,
                                   UniformMemory::Type::VEC4,
                                   UniformMemory::Type::VEC4,

                                   UniformMemory::Type::SCALAR,

                                   UniformMemory::newColMat(4, 4)
                           }),

            UniformMemory::Type::SCALAR,
            UniformMemory::newArray(MAX_POINT_LIGHTS, UniformMemory::newStruct({
                                                                   UniformMemory::Type::VEC3,

                                                                   UniformMemory::Type::VEC4,
                                                                   UniformMemory::Type::VEC4,
                                                                   UniformMemory::Type::VEC4,

                                                                   UniformMemory::Type::SCALAR,
                                                                   UniformMemory::Type::SCALAR,
                                                                   UniformMemory::Type::SCALAR,

                                                                   UniformMemory::Type::SCALAR
                                                           })),

            UniformMemory::Type::SCALAR,
            UniformMemory::newArray(MAX_SPOT_LIGHTS, UniformMemory::newStruct({
                                                                  UniformMemory::Type::VEC3,
                                                                  UniformMemory::Type::VEC3,

                                                                  UniformMemory::Type::SCALAR,
                                                                  UniformMemory::Type::SCALAR,

                                                                  UniformMemory::Type::VEC4,
                                                                  UniformMemory::Type::VEC4,
                                                                  UniformMemory::Type::VEC4,

                                                                  UniformMemory::Type::SCALAR,
                                                                  UniformMemory::Type::SCALAR,
                                                                  UniformMemory::Type::SCALAR,

                                                                  UniformMemory::Type::SCALAR,
                                                                  UniformMemory::Type::SCALAR,

                                                                  UniformMemory::newColMat(4, 4)
                                                          }))
    });

    for (Shader s : shaders) {
        lightUBO.attachToShader(s, "Lights");
    }

    lightUBO.generate();
    lightUBO.bind();
    lightUBO.initNullData(GL_STATIC_DRAW);
    lightUBO.bindRange();

    lightUBO.startWrite();

    lightUBO.writeElement<glm::vec3>(&dirLight->direction);
    lightUBO.writeElement<glm::vec4>(&dirLight->ambient);
    lightUBO.writeElement<glm::vec4>(&dirLight->diffuse);
    lightUBO.writeElement<glm::vec4>(&dirLight->specular);
    lightUBO.writeElement<float>(&dirLight->br.max.z);
    lightUBO.writeArrayContainer<glm::mat4, glm::vec4>(&dirLight->lightSpaceMatrix, 4);

    // point lights
    noPointLights = std::min<unsigned int>(pointLights.size(), MAX_POINT_LIGHTS);
    lightUBO.writeElement<unsigned int>(&noPointLights);
    unsigned int i = 0;
    for (; i < noPointLights; i++) {
        lightUBO.writeElement<glm::vec3>(&pointLights[i]->position);
        lightUBO.writeElement<glm::vec4>(&pointLights[i]->ambient);
        lightUBO.writeElement<glm::vec4>(&pointLights[i]->diffuse);
        lightUBO.writeElement<glm::vec4>(&pointLights[i]->specular);
        lightUBO.writeElement<float>(&pointLights[i]->k0);
        lightUBO.writeElement<float>(&pointLights[i]->k1);
        lightUBO.writeElement<float>(&pointLights[i]->k2);
        lightUBO.writeElement<float>(&pointLights[i]->farPlane);
    }
    lightUBO.advanceArray(MAX_POINT_LIGHTS - i); // advance to finish array

    noSpotLights = std::min<unsigned int>(spotLights.size(), MAX_SPOT_LIGHTS);
    lightUBO.writeElement<unsigned int>(&noSpotLights);
    for (i = 0; i < noSpotLights; i++) {
        lightUBO.writeElement<glm::vec3>(&spotLights[i]->position);
        lightUBO.writeElement<glm::vec3>(&spotLights[i]->direction);
        lightUBO.writeElement<float>(&spotLights[i]->cutOff);
        lightUBO.writeElement<float>(&spotLights[i]->outerCutOff);
        lightUBO.writeElement<glm::vec4>(&spotLights[i]->ambient);
        lightUBO.writeElement<glm::vec4>(&spotLights[i]->diffuse);
        lightUBO.writeElement<glm::vec4>(&spotLights[i]->specular);
        lightUBO.writeElement<float>(&spotLights[i]->k0);
        lightUBO.writeElement<float>(&spotLights[i]->k1);
        lightUBO.writeElement<float>(&spotLights[i]->k2);
        lightUBO.writeElement<float>(&spotLights[i]->nearPlane);
        lightUBO.writeElement<float>(&spotLights[i]->farPlane);
        lightUBO.writeArrayContainer<glm::mat4, glm::vec4>(&spotLights[i]->lightSpaceMatrix, 4);
    }

    lightUBO.clear();
}

void Scene::processInput(float dt) {
    if (state == WindowStates::ACTIVE) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (activeCamera != -1 && activeCamera < cameras.size()) {
            double dx = Mouse::getDX(), dy = Mouse::getDY();
            if (dx != 0 || dy != 0) {
                cameras[activeCamera]->updateCameraDirection(dx, dy);
            }

            double scrollDy = Mouse::getScrollDY();
            if (scrollDy != 0) {
                cameras[activeCamera]->updateCameraZoom(scrollDy);
            }

            if (Keyboard::key(GLFW_KEY_W)) {
                cameras[activeCamera]->updateCameraPos(CameraDirection::FORWARD, dt);
            }
            if (Keyboard::key(GLFW_KEY_S)) {
                cameras[activeCamera]->updateCameraPos(CameraDirection::BACKWARD, dt);
            }
            if (Keyboard::key(GLFW_KEY_D)) {
                cameras[activeCamera]->updateCameraPos(CameraDirection::RIGHT, dt);
            }
            if (Keyboard::key(GLFW_KEY_A)) {
                cameras[activeCamera]->updateCameraPos(CameraDirection::LEFT, dt);
            }
            if (Keyboard::key(GLFW_KEY_SPACE)) {
                cameras[activeCamera]->updateCameraPos(CameraDirection::UP, dt);
            }
            if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
                cameras[activeCamera]->updateCameraPos(CameraDirection::DOWN, dt);
            }

            view = cameras[activeCamera]->getViewMatrix();
            projection = glm::perspective(
                    glm::radians(cameras[activeCamera]->getZoom()),        // FOV
                    (float) scrWidth / (float) scrHeight,                    // aspect ratio
                    0.1f, 100.0f                                        // near and far bounds
            );
            textProjection = glm::ortho(0.0f, (float) scrWidth, 0.0f, (float) scrHeight);

            cameraPos = cameras[activeCamera]->cameraPos;
        }

        if (Keyboard::keyWentDown(GLFW_KEY_ESCAPE)) {
            state = WindowStates::MENU;
        }
    }
    if (state == WindowStates::MENU) {
        if (Keyboard::keyWentDown(GLFW_KEY_ESCAPE)) {
            state = WindowStates::ACTIVE;
        }
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Scene::update() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(bg[0], bg[1], bg[2], bg[4]);
    defaultFBO.clear();
}

void Scene::newFrame(Box &box) {
    box.positions.clear();
    box.sizes.clear();

    octree->processPending();
    octree->update(box);

    glfwSwapBuffers(window);
    glfwPollEvents();

}

void Scene::renderShader(Shader shader, bool applyLighting) {
    shader.activate();

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.set3Float("viewPos", cameraPos);

    if (applyLighting) {
        unsigned int textureIdx = 31;

        dirLight->render(shader, textureIdx--);

        unsigned int noLights = pointLights.size();
        unsigned int noActiveLights = 0;
        for (unsigned int i = 0; i < noLights; i++) {
            if (States::indexActive(&activePointLights, i)) {
                pointLights[i]->render(shader, noActiveLights, textureIdx--);
                noActiveLights++;
            }
        }
        shader.setInt("noPointLights", noActiveLights);

        noLights = spotLights.size();
        noActiveLights = 0;
        for (unsigned int i = 0; i < noLights; i++) {
            if (States::indexActive(&activeSpotLights, i)) {// i'th spot light active
                spotLights[i]->render(shader, noActiveLights, textureIdx--);
                noActiveLights++;
            }
        }
        shader.setInt("noSpotLights", noActiveLights);
    }
}

void Scene::renderDirLightShader(Shader shader) {
    shader.activate();
    shader.setMat4("lightSpaceMatrix", dirLight->lightSpaceMatrix);
}

void Scene::renderPointLightShader(Shader shader, unsigned int idx) {
    shader.activate();
    for (unsigned int i = 0; i < 6; i++) {
        shader.setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", pointLights[idx]->lightSpaceMatrices[i]);
    }
    shader.set3Float("lightPos", pointLights[idx]->position);
    shader.setFloat("farPlane", pointLights[idx]->farPlane);
}

void Scene::renderSpotLightShader(Shader shader, unsigned int idx) {
    shader.activate();
    shader.setMat4("lightSpaceMatrix", spotLights[idx]->lightSpaceMatrix);
    shader.set3Float("lightPos", spotLights[idx]->position);
    shader.setFloat("farPlane", spotLights[idx]->farPlane);
}

void Scene::renderInstances(std::string modelId, Shader shader, float dt) {
    void* val = avl_get(models, (void*)modelId.c_str());
    if (val) {
        shader.activate();
        ((Model*)val)->render(shader, dt, this);
    }
}

void Scene::renderText(std::string font, Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color) {
    void* val = avl_get(fonts, (void*)font.c_str());
    if (val) {
        shader.activate();
        shader.setMat4("projection", textProjection);
        ((TextRenderer*)val)->render(shader, text, x, y, scale, color);
    }
}

void Scene::cleanup() {
    instances.cleanup();
    avl_postorderTraverse(models, [](AVL* node) -> void {
        ((Model*)node->val)->cleanup();
    });
    avl_free(models);
//    avl_postorderTraverse(fonts, [](AVL* node) -> void {
//        ((TextRenderer*)node->val)->cleanup();
//    });
//    avl_free(fonts);

    octree->destroy();
    glfwTerminate();
}

bool Scene::shouldClose() {
    return glfwWindowShouldClose(window);
}

Camera* Scene::getActiveCamera() {
    return (activeCamera >= 0 && activeCamera < cameras.size()) ? cameras[activeCamera] : nullptr;
}

void Scene::setShouldClose(bool shouldClose) {
    glfwSetWindowShouldClose(window, shouldClose);
}

void Scene::setWindowColor(float r, float g, float b, float a) {
    bg[0] = r;
    bg[1] = g;
    bg[2] = b;
    bg[3] = a;
}

void Scene::registerModel(Model* model) {
    models = avl_insert(models, (void*)model->id.c_str(), model);
}

RigidBody* Scene::generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot) {
    void* val = avl_get(models, (void*)modelId.c_str());
    if (val) {
        Model* model = (Model*)val;
        RigidBody* rb = model->generateInstance(size, mass, pos, rot);
        if (rb) {
            std::string id = generateId();
            rb->instanceId = id;
            instances.insert(rb->instanceId, rb);
            octree->addToPending(rb, model);
            return rb;
        }
    }
    return nullptr;
}

void Scene::initInstances() {
    avl_inorderTraverse(models, [](AVL* node) -> void {
        ((Model*)node->val)->initInstances();
    });
}

void Scene::loadModels() {
    avl_inorderTraverse(models, [](AVL* node) -> void {
        ((Model*)node->val)->init();
    });
}

void Scene::removeInstance(std::string instanceId) {
    RigidBody* instance = instances[instanceId];
    std::string targetModel = instance->modelId;
    Model* model = (Model*)avl_get(models, (void*)targetModel.c_str());
    model->removeInstance(instanceId);
    instances[instanceId] = NULL;
    instances.erase(instanceId);
    free(instance);
}

void Scene::markForDeletion(std::string instanceId) {
    RigidBody* instance = instances[instanceId];
    States::activate(&instance->state, INSTANCE_DEAD);
    instancesToDelete.push_back(instance);
}

void Scene::clearDeadInstances() {
    for (RigidBody* rb : instancesToDelete) {
        removeInstance(rb->instanceId);
    }
    instancesToDelete.clear();
}

std::string Scene::generateId() {
    for (int i = currentId.length() - 1; i >= 0; i--) {
        if ((int)currentId[i] != (int)'z') {
            currentId[i] = (char)(((int)currentId[i]) + 1);
            break;
        }
        else {
            currentId[i] = 'a';
        }
    }
    return currentId;
}

