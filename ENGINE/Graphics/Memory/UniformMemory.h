//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_UNIFORMMEMORY_H
#define MARCHING_CUBES_UNIFORMMEMORY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VertexMemory.h"
#include "../Shader/Shader.h"
#include <vector>
#include <string>


namespace UniformMemory {
    const int wordSize = 4;

    enum class Type : unsigned char {
        SCALAR = 0,
        VEC2,
        VEC3,
        VEC4,
        ARRAY,
        STRUCT,
        INVALID
    };

    unsigned int roundPow2(unsigned int val, unsigned char number);

    class Element {
    public:
        Element(Type type = Type::SCALAR);
        std::string typeStr();
        unsigned int alignPow2();
        unsigned int calcSize();
        unsigned int calcPadSize();

        Type type;
        unsigned int baseAlign;
        unsigned int length;
        std::vector<Element> list;
    };

    inline Element newScalar();
    inline Element newVec(unsigned char dimension);
    inline Element newArray(unsigned int length, Element arrElement);
    inline Element newColMat(unsigned char cols, unsigned char rows);
    inline Element newColMatArray(unsigned int noMatrices, unsigned char cols, unsigned char rows);
    inline Element newRowMat(unsigned char rows, unsigned char cols);
    inline Element newRowMatArray(unsigned int noMatrices, unsigned char rows, unsigned char cols);
    inline Element newStruct(std::vector<Element> subelements);

    class UBO : public BufferObject {
    public:
        UBO(GLuint bindingPos);
        UBO(GLuint bindingPos, std::vector<Element> elements);

        void attachToShader(Shader shader, std::string name);
        void initNullData(GLenum usage);
        void bindRange(GLuint offset = 0);
        void addElement(Element element);
        void startWrite();

        template<class T>
        void writeElement(T* data);

        template<class T>
        void writeArray(T* first, unsigned int n);

        template<class T, class V>
        void writeArrayContainer(T* container, unsigned int n);

        void advanceCursor(unsigned int n);
        void advanceArray(unsigned int numberElements);

        Element getNextElement();

        unsigned int calculateSize();

        bool pop();

        Element block;
        unsigned int calculatedSize;
        GLuint bindingPos;

        GLuint offset;
        GLuint poppedOffset;
        std::vector<std::pair<unsigned int, Element*>> indexStack;
        int currentDepth;
    };




};


#endif //MARCHING_CUBES_UNIFORMMEMORY_H
