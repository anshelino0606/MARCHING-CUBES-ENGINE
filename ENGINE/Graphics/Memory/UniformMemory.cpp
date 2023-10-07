//
// Created by Anhelina Modenko on 06.10.2023.
//

#include "UniformMemory.h"


inline unsigned int UniformMemory::roundPow2(unsigned int val, unsigned char n) {
    unsigned int pow2n = 0b1 << n; // = 1 * 2^n = 2^n
    unsigned int divisor = pow2n - 1; // = 0b0111...111 (n 1s)
    unsigned int rem = val & divisor;
    if (rem) {
        val += pow2n - rem;
    }

    return val;
}

std::string UniformMemory::Element::typeStr() {
    switch (type) {
        case Type::SCALAR: return "scalar";
        case Type::VEC2: return "vec2";
        case Type::VEC3: return "vec3";
        case Type::VEC4: return "vec4";
        case Type::ARRAY: return "array<" + list[0].typeStr() + ">";
        case Type::STRUCT: return "struct";
        default: return "invalid";
    };
}

unsigned int UniformMemory::Element::alignPow2() {
    switch (baseAlign) {
        case 2: return 1;
        case 4: return 2;
        case 8: return 3;
        case 16: return 4;
        default: return 0;
    };
}

    unsigned int UniformMemory::Element::calcSize() {
        switch (type) {
            case Type::SCALAR:
                return wordSize;
            case Type::VEC2:
                return 2 * wordSize;
            case Type::VEC3:
                return 3 * wordSize;
            case Type::VEC4:
                return 4 * wordSize;
            case Type::ARRAY:
            case Type::STRUCT:
                return calcPadSize();
            default:
                return 0;
        };
    }

    unsigned int UniformMemory::Element::calcPadSize() {
        unsigned int offset = 0;

        switch (type) {
            case Type::ARRAY:
                return length * roundPow2(list[0].calcSize(), alignPow2());
            case Type::STRUCT:
                for (Element e: list) {
                    offset = roundPow2(offset, e.alignPow2());
                    offset += e.calcSize();
                }
                return offset;
            case Type::SCALAR:
            case Type::VEC2:
            case Type::VEC3:
            case Type::VEC4:
            default:
                return calcSize();
        }
    }

UniformMemory::Element::Element(UniformMemory::Type type)
: type(type), length(0), list(0) {
        switch (type) {
            case Type::SCALAR:
                baseAlign = wordSize;
                break;
            case Type::VEC2:
                baseAlign = 2 * wordSize;
                break;
            case Type::VEC3:
            case Type::VEC4:
                baseAlign = 4 * wordSize;
                break;
            default:
                baseAlign = 0;
                break;
        }
}



UniformMemory::UBO::UBO(GLuint bindingPos)
        : BufferObject(GL_UNIFORM_BUFFER),
          block(newStruct({})),
          calculatedSize(0),
          bindingPos(bindingPos) {}

UniformMemory::UBO::UBO(GLuint bindingPos, std::vector<Element> elements)
        : BufferObject(GL_UNIFORM_BUFFER),
          block(newStruct(elements)),
          calculatedSize(0),
          bindingPos(bindingPos) {}

void UniformMemory::UBO::attachToShader(Shader shader, std::string name) {
    GLuint blockIdx = glGetUniformBlockIndex(shader.id, name.c_str());
    glUniformBlockBinding(shader.id, blockIdx, bindingPos);
}

void UniformMemory::UBO::initNullData(GLenum usage) {
    if (!calculatedSize) {
        calculatedSize = calculateSize();
    }
    glBufferData(type, calculatedSize, NULL, usage);
}

void UniformMemory::UBO::bindRange(GLuint offset) {
    if (!calculatedSize) {
        calculatedSize = calculateSize();
    }
    glBindBufferRange(type, bindingPos, val, offset, calculatedSize);
}

unsigned int UniformMemory::UBO::calculateSize() {
    return block.calcPadSize();
}

void UniformMemory::UBO::addElement(UniformMemory::Element element) {
    block.list.push_back(element);
    if (element.baseAlign > block.baseAlign) {
        block.baseAlign = element.baseAlign;
    }
    block.length++;
}

// initialize iterator
void UniformMemory::UBO::startWrite() {
    currentDepth = 0;
    offset = 0;
    poppedOffset = 0;
    indexStack.clear();
    indexStack.push_back({ 0, &block });
}

// next element in iteration
UniformMemory::Element UniformMemory::UBO::getNextElement() {
    // highest level struct popped, stack is empty
    if (currentDepth < 0) {
        return Type::INVALID;
    }

    // get current deepest array/struct (last element in the stack)
    Element* currentElement = indexStack[currentDepth].second;

    // get the element at the specified index within that iterable
    if (currentElement->type == Type::STRUCT) {
        currentElement = &currentElement->list[indexStack[currentDepth].first];
    }
    else { // array
        currentElement = &currentElement->list[0];
    }

    // traverse down to deepest array/struct
    while (currentElement->type == Type::STRUCT || currentElement->type == Type::ARRAY) {
        currentDepth++;
        indexStack.push_back({ 0, currentElement });
        currentElement = &currentElement->list[0];
    }

    // now have current element (not an iterable)
    // pop from stack if necessary
    poppedOffset = roundPow2(offset, currentElement->alignPow2()) + currentElement->calcSize();
    if (!pop()) {
        // no items popped
        poppedOffset = 0;
    }

    return *currentElement;
}

bool UniformMemory::UBO::pop() {
    bool popped = false;

    for (int i = currentDepth; i >= 0; i--) {
        int advancedIdx = ++indexStack[i].first; // move cursor forward in the iterable
        if (advancedIdx >= indexStack[i].second->length) {
            // iterated through entire array or struct
            // pop iterable from the stack
            poppedOffset = roundPow2(poppedOffset, indexStack[i].second->alignPow2());
            indexStack.erase(indexStack.begin() + i);
            popped = true;
            currentDepth--;
        }
        else {
            break;
        }
    }

    return popped;
}

//template<typename T>
//void UniformMemory::UBO::writeElement(T* data) {
//    Element element = getNextElement();
//    //std::cout << element.typeStr() << "--" << element.baseAlign << "--" << offset << "--";
//    offset = roundPow2(offset, element.alignPow2());
//    //std::cout << offset << std::endl;
//
//    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), data);
//
//    if (poppedOffset) {
//        offset = poppedOffset;
//    }
//    else {
//        offset += element.calcSize();
//    }
//}

template<typename T>
void UniformMemory::UBO::writeArray(T* first, unsigned int n) {
    for (int i = 0; i < n; i++) {
        writeElement<T>(&first[i]);
    }
}

//template<typename T, typename V>
//void UniformMemory::UBO::writeArrayContainer(T* container, unsigned int n) {
//
//}

void UniformMemory::UBO::advanceCursor(unsigned int n) {
    // skip number of elements
    for (int i = 0; i < n; i++) {
        Element element = getNextElement();
        offset = roundPow2(offset, element.alignPow2());
        if (poppedOffset) {
            offset = poppedOffset;
        }
        else {
            offset += element.calcSize();
        }
    }
}

void UniformMemory::UBO::advanceArray(unsigned int noElements) {
    if (currentDepth < 0) {
        return;
    }

    Element* currentElement = indexStack[currentDepth].second;

    // get the next array
    if (currentElement->type == Type::STRUCT) {
        currentElement = &currentElement->list[indexStack[currentDepth].first];

        unsigned int depthAddition = 0;
        std::vector<std::pair<unsigned int, Element*>> stackAddition;

        // go to next array
        while (currentElement->type == Type::STRUCT) {
            depthAddition++;
            stackAddition.push_back({ 0, currentElement });
            currentElement = &currentElement->list[0];
        }

        if (currentElement->type != Type::ARRAY) {
            // did not find an array (reached primitive)
            return;
        }

        // found array, apply changes
        currentDepth += depthAddition + 1; // + 1 for the array
        indexStack.insert(indexStack.end(), stackAddition.begin(), stackAddition.end());
        indexStack.push_back({ 0, currentElement }); // push array to stack
    }

    // at an array, advance number of elements
    unsigned int finalIdx = indexStack[currentDepth].first + noElements;
    unsigned int advanceCount = noElements;
    if (finalIdx >= indexStack[currentDepth].second->length) {
        // advance to the end of array
        advanceCount = indexStack[currentDepth].second->length - indexStack[currentDepth].first;
    }

    // advance offset
    offset += advanceCount * roundPow2(currentElement->list[0].calcSize(), currentElement->alignPow2());
    // advance cursor in stack
    indexStack[currentDepth].first += advanceCount;

    // pop from stack
    poppedOffset = offset;
    if (pop()) {
        // item(s) popped
        offset = poppedOffset;
    }
}
