//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_FRAMEMEMORY_H
#define MARCHING_CUBES_FRAMEMEMORY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Texture/Texture.h"
#include "../Cubemap/Cubemap.h"

#include <vector>

class FrameBuffer {
public:
    FrameBuffer();
    FrameBuffer(GLuint width, GLuint height, GLbitfield combo);

    void generate();
    void setViewport();
    void disableColorBuffer();
    void bind();
    void clear();
    void activate();
    void activateRBO(GLenum type, GLenum format);
    void activateTexture(GLenum attachType, GLenum format, GLenum type);
    void attachTexture(GLenum type, Texture texture);
    void activateCubemap(GLenum attachType, GLenum format, GLenum type);
    void cleanup();

    GLuint val;
    GLuint width;
    GLuint height;
    GLbitfield bitCombo;

    std::vector<GLuint> rbos;
    std::vector<Texture> textures;
    Cubemap cubemap;
};

class RenderBuffer {
public:
    void generate();
    void bind();
    void allocate(GLenum format, GLuint width, GLuint height);
    void cleanup();

    GLuint val;
};

#endif //MARCHING_CUBES_FRAMEMEMORY_H
