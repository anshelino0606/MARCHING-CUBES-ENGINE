#ifndef MARCHING_CUBES_LIGHT_HPP
#define MARCHING_CUBES_LIGHT_HPP

#include <glm/glm.hpp>

#include "cube.hpp"
#include "../Graphics/Material/Material.h"


class LampLight : public Cube {
public:
    LampLight(unsigned int maxNoInstances, glm::vec3 color = glm::vec3(1.0f))
            : Cube(maxNoInstances, Material::white_rubber) {
        id = "lamp";
        this->color = color;
    }

    glm::vec3 color;
};

#endif