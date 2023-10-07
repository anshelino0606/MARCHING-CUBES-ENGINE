//
// Created by Anhelina Modenko on 06.10.2023.
//

#include "VertexMemory.h"

BufferObject::BufferObject(GLenum type)
: type(type) {}

void BufferObject::generate() {
    glGenBuffers(1, &val);
}

void BufferObject::bind() {
    glBindBuffer(type, val);
}

//template<typename T>
//void BufferObject::setData(GLuint noElements, T* data, GLenum usage) {
//
//}

//template<typename T>
//void BufferObject::updateData(GLintptr offset, GLuint noElements, T* data) {
//
//}

//template<typename T>
//void BufferObject::setAttribPointer(GLuint index, GLint size, GLenum type, GLuint stride, GLuint offset, GLuint divisor) {
//
//}

void BufferObject::clear() {
    glBindBuffer(type, 0);
}

void BufferObject::cleanup() {
    glDeleteBuffers(1, &val);
}

BufferObject& ArrayObject::operator[](const char* key) {
    return buffers[key];
}

void ArrayObject::generate() {
    glGenVertexArrays(1, &val);
}

void ArrayObject::bind() {
    glBindVertexArray(val);
}

void ArrayObject::draw(GLenum mode, GLuint first, GLuint count) {
    glDrawArrays(mode, first, count);
}

void ArrayObject::draw(GLenum mode, GLuint count, GLenum type, const void* indices, GLuint instanceCount) {
    glDrawElementsInstanced(mode, count, type, indices, instanceCount);
}

void ArrayObject::cleanup() {
    glDeleteVertexArrays(1, &val);
    for (auto& pair : buffers) {
        pair.second.cleanup();
    }
}

void ArrayObject::clear() {
    glBindVertexArray(0);
}