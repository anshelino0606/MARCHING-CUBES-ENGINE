//
// Created by Anhelina Modenko on 03.07.2023.
//

#ifndef OPENGL_SCREEN_H
#define OPENGL_SCREEN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Screen {
public:
    Screen();

    static unsigned int SCR_WIDTH;
    static unsigned int SCR_HEIGHT;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    bool init();

    void setParameters();

    void update();
    void newFrame();

    bool shouldClose();
    void setShouldClos(bool shouldClose);
private:
    GLFWwindow* window;
};


#endif //OPENGL_SCREEN_H
