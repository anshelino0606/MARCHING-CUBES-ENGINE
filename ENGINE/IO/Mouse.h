//
// Created by Anhelina Modenko on 01.07.2023.
//

#ifndef OPENGL_MOUSE_H
#define OPENGL_MOUSE_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

class Mouse {
public:
    static void cursorPosCallback(GLFWwindow* window, double _x, double _y);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mouseWheelCallback(GLFWwindow* window, double _dx, double _dy);

    static double getMouseX();
    static double getMouseY();

    static double getDX();
    static double getDY();

    static double getScrollDX();
    static double getScrollDY();

    static bool button(int button);
    static bool buttonChanged(int button);
    static bool buttonWentUp(int button);
    static bool buttonWentDown(int button);
private:
    static double x;
    static double y;

    static double lastX;
    static double lastY;

    static double dx;
    static double dy;

    static double scrollDX;
    static double scrollDY;

    static bool firstMouse;

    static bool buttons[];
    static bool buttonsChanged[];
    static float mouseSensitivity;

};


#endif //OPENGL_MOUSE_H
