#define GLFW_INCLUDE_GLCOREARB
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "LIBRARIES/imgui/imgui.h"
#include "LIBRARIES/imgui/backends/imgui_impl_glfw.h"
#include "LIBRARIES/imgui/backends/imgui_impl_opengl3.h"

#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>

#include "ENGINE/Graphics/Memory/FrameMemory.h"
#include "ENGINE/Graphics/Memory/UniformMemory.h"

#include "ENGINE/Models/cube.hpp"
#include "ENGINE/Models/light.hpp"
#include "ENGINE/Models/sphere.hpp"
#include "ENGINE/Models/box.hpp"
#include "ENGINE/Models/plane.hpp"

#include "ENGINE/Graphics/Model/Model.h"

#include "ENGINE/Graphics/Shader/Shader.h"
#include "ENGINE/Graphics/Texture/Texture.h"
#include "ENGINE/Graphics/Light/Light.h"
#include "ENGINE/Graphics/Cubemap/Cubemap.h"
#include "ENGINE/Graphics/Text/TextRenderer.h"

#include "ENGINE/Physics/Environment/Environment.h"

#include "ENGINE/IO/Keyboard.h"
#include "ENGINE/IO/Mouse.h"
#include "ENGINE/IO/Camera.h"

#include "ENGINE/Algorithms/States/States.h"

#include "Scene.h"

Scene scene;

void processInput(double dt);
void renderScene(Shader shader);

Camera cam;

//Joystick mainJ(0);

double dt = 0.0f; // tme btwn frames
double lastFrame = 0.0f; // time of last frame

Sphere sphere(10);
Cube cube(10);
LampLight lamp(4);

std::string Shader::defaultDirectory = "/Users/anhelinamodenko/CLionProjects/MARCHING-CUBES/ASSETS/Shaders";

#include "ENGINE/Physics/CollisionMesh/CollisionMesh.h"

int main() {
    std::cout << "Hello, OpenGL!" << std::endl;

    // construct scene
    scene = Scene(3, 3, "OpenGL Tutorial", 1680*2, 1020*2);
    // test if GLFW successfully started and created window
    if (!scene.init()) {
        std::cout << "Could not open window" << std::endl;
        scene.cleanup();
        return -1;
    }

    // set camera
    scene.cameras.push_back(&cam);
    scene.activeCamera = 0;

    // SHADERS===============================
    Shader::loadIntoDefault("defaultHead.geom");

    Shader shader(true, "Instanced/instanced.vert", "object.frag");
    Shader boxShader(false, "Instanced/box.vert", "Instanced/box.frag");

    Shader dirShadowShader(false, "Shadows/dirSpot.vert",
                           "Shadows/dirShadow.frag");
    Shader spotShadowShader(false, "Shadows/dirSpot.vert",
                            "Shadows/pointShadow.frag");
    Shader pointShadowShader(false, "Shadows/pointShadow.vert",
                             "Shadows/pointShadow.frag",
                             "Shadows/pointShadow.geom");

    Shader::clearDefault();


    // FONTS===============================
//    TextRenderer font(32);
//    if (!scene.registerFont(&font, "comic", "ASSETS/Fonts/comic.ttf")) {
//        std::cout << "Could not load font" << std::endl;
//    }

    // MODELS==============================
    scene.registerModel(&lamp);

    scene.registerModel(&sphere);

    scene.registerModel(&cube);


    Box box;
    box.init();

    // load all model data
    scene.loadModels();

    // LIGHTS==============================

    // directional light
    DirLight dirLight(glm::vec3(-0.2f, -0.9f, -0.2f),
                      glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
                      glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),
                      glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),
                      Bounds(glm::vec3(-20.0f, -20.0f, 0.5f), glm::vec3(20.0f, 20.0f, 50.0f)));
    scene.dirLight = &dirLight;

    // point lights
    glm::vec3 pointLightPositions[] = {
            glm::vec3(0.0f, 5.0f, 0.0f),
            glm::vec3(0.0f,  15.0f,  0.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    glm::vec4 specular = glm::vec4(1.0f);
    float k0 = 1.0f;
    float k1 = 0.0014f;
    float k2 = 0.000007f;

    PointLight pointLights[4];

    for (unsigned int i = 0; i < 1; i++) {
        pointLights[i] = PointLight(
                pointLightPositions[i],
                k0, k1, k2,
                ambient, diffuse, specular,
                0.5f, 50.0f
        );
        // create physical model for each lamp
        scene.generateInstance(lamp.id, glm::vec3(2.0f, 2.f, 2.0f), 0.5f, pointLightPositions[i]);
        // add lamp to scene's light source
        scene.pointLights.push_back(&pointLights[i]);
        // activate lamp in scene
        States::activateIndex(&scene.activePointLights, i);
    }

    // spot light
    SpotLight spotLight(
            //glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
            cam.cameraPos, cam.cameraFront, cam.cameraUp,
            glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
            1.0f, 0.0014f, 0.000007f,
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f),
            0.1f, 100.0f
    );
    scene.spotLights.push_back(&spotLight);
    scene.activeSpotLights = 1; // 0b00000001

    scene.generateInstance(cube.id, glm::vec3(30.0f, 0.01f, 30.0f), 100.0f, glm::vec3(0.0f, -4.0f, 0.0f));
//

    // instantiate instances
    scene.initInstances();

    // finish preparations (octree, etc)
    scene.prepare(box, { shader });

    scene.variableLog["time"] = (double)0.0;

    scene.defaultFBO.bind(); // bind default framebuffer

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(scene.window, true);
    ImGui_ImplOpenGL3_Init();



    while (!scene.shouldClose()) {
        // calculate dt
        double currentTime = glfwGetTime();
        dt = currentTime - lastFrame;
        lastFrame = currentTime;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

//        scene.variableLog["time"] += dt;
        scene.variableLog["fps"] = 1 / dt;

        // update screen values
        scene.update();

        // process input
        processInput(dt);


        // remove launch objects if too far
        for (int i = 0; i < sphere.currentNoInstances; i++) {
            if (glm::length(cam.cameraPos - sphere.instances[i]->pos) > 250.0f) {
                scene.markForDeletion(sphere.instances[i]->instanceId);
            }
        }

        // render scene to dirlight FBO
        dirLight.shadowFBO.activate();
        scene.renderDirLightShader(dirShadowShader);
        renderScene(dirShadowShader);

        //// render scene to point light FBOs
//        for (unsigned int i = 0, len = scene.pointLights.size(); i < len; i++) {
//            if (States::indexActive(&scene.activePointLights, i)) {
//                scene.pointLights[i]->shadowFBO.activate();
//                scene.renderPointLightShader(pointShadowShader, i);
//                renderScene(pointShadowShader);
//            }
//        }

        //// render scene to spot light FBOs
//        for (unsigned int i = 0, len = scene.spotLights.size(); i < len; i++) {
//            if (States::indexActive(&scene.activeSpotLights, i)) {
//                scene.spotLights[i]->shadowFBO.activate();
//                scene.renderSpotLightShader(spotShadowShader, i);
//                renderScene(spotShadowShader);
//            }
//        }

        // render scene normally
        scene.defaultFBO.activate();
        scene.renderShader(shader);
        renderScene(shader);

        // render boxes
//        scene.renderShader(boxShader, false);
//        box.render(boxShader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // send new frame to window
        scene.newFrame(box);



        // clear instances that have been marked for deletion
        scene.clearDeadInstances();

    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // clean up objects
    scene.cleanup();
    std::cout << "DONE" << std::endl;
    return 0;
}

void renderScene(Shader shader) {
    if (sphere.currentNoInstances > 0) {
        scene.renderInstances(sphere.id, shader, dt);
    }

    scene.renderInstances(cube.id, shader, dt);

    scene.renderInstances(lamp.id, shader, dt);

}

void launchItem(float dt) {
    RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(0.3f), 3.0f, cam.cameraPos);
    if (rb) {
        // instance generated successfully
        rb->transferEnergy(5.0f, cam.cameraFront);
        rb->applyAcceleration(Environment::gravitationalAcceleration);
    }
}


void processInput(double dt) {
    // process input with cameras
    scene.processInput(dt);

    // close window
    if (Keyboard::key(GLFW_KEY_X)) {
        scene.setShouldClose(true);
    }


    // update flash light
    if (States::indexActive(&scene.activeSpotLights, 0)) {
        scene.spotLights[0]->position = scene.getActiveCamera()->cameraPos;
        scene.spotLights[0]->direction = scene.getActiveCamera()->cameraFront;
        scene.spotLights[0]->up = scene.getActiveCamera()->cameraUp;
        scene.spotLights[0]->updateMatrices();
    }

    if (Keyboard::keyWentDown(GLFW_KEY_L)) {
        States::toggleIndex(&scene.activeSpotLights, 0); // toggle spot light
    }

    // launch sphere
    if (Keyboard::keyWentDown(GLFW_KEY_F)) {
        launchItem(dt);
    }


    // determine if each lamp should be toggled
    for (int i = 0; i < 4; i++) {
        if (Keyboard::keyWentDown(GLFW_KEY_1 + i)) {
            //States::toggleIndex(&scene.activePointLights, i);
        }
    }
}