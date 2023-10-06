//
// Created by Anhelina Modenko on 01.07.2023.
//

#ifndef OPENGL_KEYBOARD_H
#define OPENGL_KEYBOARD_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

class Keyboard {
public:
    // key state callbak
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    // accessorts
    static bool key(int key);
    static bool keyChanged(int key);
    static bool keyWentUp(int key);
    static bool keyWentDown(int key);
private:
    static bool keys[];
    static bool keysChanged[];
};


#endif //OPENGL_KEYBOARD_H
