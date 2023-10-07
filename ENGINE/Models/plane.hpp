#ifndef MARCHING_CUBES_PLANE_HPP
#define MARCHING_CUBES_PLANE_HPP

#include "../Graphics/Model/Model.h"

class Plane : public Model {
public:
    Plane()
            : Model("plane", 1, CONST_INSTANCES) {}

    void init(std::vector<Texture> textures) {
        int noVertices = 4;

        float quadVertices[] = {
                 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // top right
                -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 1.0f, // top left
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom left
                 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 0.0f  // bottom right
        };
        float collisionVertices[] = {
                 0.5f,  0.5f, 0.0f,
                -0.5f,  0.5f, 0.0f,
                -0.5f, -0.5f, 0.0f,
                 0.5f, -0.5f, 0.0f
        };

        unsigned int indices[] = {
                0, 1, 3,
                1, 2, 3
        };

        Bounds br(glm::vec3(0.0f), 1 / sqrt(2.0f));

        Mesh result = processMesh(br, noVertices, quadVertices,
                               6, indices,
                               true, noVertices, collisionVertices,
                               2, indices);

        result.setupTextures(textures);
        addMesh(&result);
    }
};

#endif