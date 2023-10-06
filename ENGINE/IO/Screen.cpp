//
// Created by Anhelina Modenko on 03.07.2023.
//

#include "Screen.h"
#include "Keyboard.h"
#include "Mouse.h"

unsigned int Screen::SCR_WIDTH = 800;
unsigned int Screen::SCR_HEIGHT = 600;

Screen::Screen()
: window(nullptr) {}

void Screen::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    Screen::SCR_WIDTH = width;
    Screen::SCR_HEIGHT = height;
}

bool Screen::init() {
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", nullptr, nullptr);
    if (window == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window);
    return true;
}

void Screen::setParameters() {
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, Keyboard::keyCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
    glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
    glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

    glEnable(GL_DEPTH_TEST);
}

void Screen::update() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::newFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Screen::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Screen::setShouldClos(bool shouldClose) {
    glfwSetWindowShouldClose(window, shouldClose);
}


