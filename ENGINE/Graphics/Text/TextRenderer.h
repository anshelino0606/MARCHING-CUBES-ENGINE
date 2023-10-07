//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_TEXT_H
#define MARCHING_CUBES_TEXT_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>

#include <string>
#include <map>
#include <iostream>

#include "../Memory/VertexMemory.h"
#include "../Shader/Shader.h"

typedef struct {
    unsigned int	textureId;	// texture id storing character
    glm::ivec2		size;		// size of char
    glm::ivec2		bearing;	// distance from origin to top left of char
    unsigned int	advance;	// distance from origin to next origin (1/64th pixels)
} Character;

class TextRenderer {
public:
    TextRenderer();
    TextRenderer(int height);

    bool loadFont(FT_Library& ft, std::string path);

    void render(Shader shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color);

    void cleanup();

private:
    int height;

    std::map<char, Character> chars;

    ArrayObject VAO;
};

#endif //MARCHING_CUBES_TEXT_H
