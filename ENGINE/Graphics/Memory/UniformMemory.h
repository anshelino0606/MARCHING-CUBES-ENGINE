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

    inline Element newScalar() {
        return Element();
    }

    inline Element newVec(unsigned char dim) {
        switch (dim) {
            case 2: return Type::VEC2;
            case 3: return Type::VEC3;
            case 4:
            default:
                return Type::VEC4;
        };
    }

    inline Element newArray(unsigned int length, Element arrElement) {
        Element ret(Type::ARRAY);
        ret.length = length;
        ret.list = { arrElement };
        ret.list.shrink_to_fit();

        ret.baseAlign = arrElement.type == Type::STRUCT ?
                        arrElement.baseAlign :
                        roundPow2(arrElement.baseAlign, 4);

        return ret;
    }

    inline Element newColMat(unsigned char cols, unsigned char rows) {
        return newArray(cols, newVec(rows));
    }

    inline Element newColMatArray(unsigned int noMatrices, unsigned char cols, unsigned char rows) {
        return newArray(noMatrices * cols, newVec(rows));
    }

    inline Element newRowMat(unsigned char rows, unsigned char cols) {
        return newArray(rows, newVec(cols));
    }

    inline Element newRowMatArray(unsigned int noMatrices, unsigned char rows, unsigned char cols) {
        return newArray(noMatrices * rows, newVec(cols));
    }

    inline Element newStruct(std::vector<Element> subelements) {
        Element ret(Type::STRUCT);
        ret.list.insert(ret.list.end(), subelements.begin(), subelements.end());
        ret.length = ret.list.size();

        // base alignment is largest of its subelements
        if (subelements.size()) {
            for (Element e : subelements) {
                if (e.baseAlign > ret.baseAlign) {
                    ret.baseAlign = e.baseAlign;
                }
            }

            ret.baseAlign = roundPow2(ret.baseAlign, 4);
        }

        return ret;
    }


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
        void writeElement(T* data) {
            Element element = getNextElement();
            //std::cout << element.typeStr() << "--" << element.baseAlign << "--" << offset << "--";
            offset = roundPow2(offset, element.alignPow2());
            //std::cout << offset << std::endl;

            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), data);

            if (poppedOffset) {
                offset = poppedOffset;
            }
            else {
                offset += element.calcSize();
            }
        }

        template<class T>
        void writeArray(T* first, unsigned int n);

        template<class T, class V>
        void writeArrayContainer(T* container, unsigned int n) {
            for (int i = 0; i < n; i++) {
                writeElement<V>(&container->operator[](i)); // container[i] translates to container + i
            }
        }

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
