//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_MESH_H
#define MARCHING_CUBES_MESH_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "../Shader/Shader.h"
#include "../Texture/Texture.h"
#include "../Material/Material.h"


//#include "../../algorithms/bounds.h"


struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;

    static std::vector<Vertex> genList(float* vertices, int noVertices);
    static void calcTanVectors(std::vector<Vertex>& list, std::vector<unsigned int>& indices);
};

class Mesh {
public:
    Mesh();
    Mesh(Bounds br);
    Mesh(Bounds br, std::vector<Texture> textures);
    Mesh(Bounds br, aiColor4D diff, aiColor4D spec);
    Mesh(Bounds br, Material m);

    Bounds br;
    CollisionMesh* collision;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    ArrayObject VAO;

    std::vector<Texture> textures;
    aiColor4D diffuse;
    aiColor4D specular;

    void loadData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool pad = false);
    void loadCollisionMesh(unsigned int noPoints, float* coordinates, unsigned int noFaces, unsigned int* indices);

    void setupTextures(std::vector<Texture> textures);
    void setupColors(aiColor4D diff, aiColor4D spec);
    void setupMaterial(Material mat);

    void render(Shader shader, unsigned int noInstances);
    void cleanup();

private:
    bool noTex;
    void setup();
};

#endif //MARCHING_CUBES_MESH_H
