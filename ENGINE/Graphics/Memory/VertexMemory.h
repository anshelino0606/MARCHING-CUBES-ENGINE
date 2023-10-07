//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_VERTEXMEMORY_H
#define MARCHING_CUBES_VERTEXMEMORY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>

class BufferObject {
public:

    BufferObject(GLenum type = GL_ARRAY_BUFFER);

    void generate();
    void bind();
    void clear();
    void cleanup();

    template<class T>
    void setData(GLuint numberElements, T* data, GLenum usage);

    template<class T>
    void updateData(GLintptr offset, GLuint noElements, T* data);

    template<class T>
    void setAttribPointer(GLuint index, GLint size, GLenum type, GLuint stride, GLuint offset, GLuint divisor = 0);

    GLuint type;
    GLuint val;
};

class ArrayObject {
public:
    void generate();
    void bind();
    static void clear();
    void cleanup();
    void draw(GLenum mode, GLuint first, GLuint count);
    void draw(GLenum mode, GLuint count, GLenum type, GLint indices, GLuint instanceCount = 1);

    BufferObject& operator[](const char* key);

private:
    GLuint val;
    std::map<const char*, BufferObject> buffers;
};


#endif //MARCHING_CUBES_VERTEXMEMORY_H
