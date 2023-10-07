//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_MODEL_H
#define MARCHING_CUBES_MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

#include "../Mesh/Mesh.h"
#include "../../Models/box.hpp"
#include "../../Physics/CollisionModel/CollisionModel.h"
#include "ENGINE/Physics/RigidBody/RigidBody.h"
#include "../../Algorithms/Bounds/Bounds.h"

#include "../Memory/VertexMemory.h"
#include "../Memory/FrameMemory.h"

#define DYNAMIC	(unsigned int)1
#define CONST_INSTANCES	(unsigned int)2
#define NO_TEX (unsigned int)4

class Scene;
class Mesh;
class Bounds;

class Model {
public:
    Model(std::string id, unsigned int maxNoInstances, unsigned int flags = 0);


    std::string id;
    std::vector<Mesh> meshes;
    CollisionModel* collision;
    std::vector<Bounds> boundingRegions;
    std::vector<RigidBody*> instances;
    unsigned int maxNoInstances;
    unsigned int currentNoInstances;
    unsigned int switches;

    virtual void init();

    void loadModel(std::string path);
    void enableCollisionModel();

    void addMesh(Mesh* mesh);

    virtual void render(Shader shader, float dt, Scene *scene);
    void cleanup();

    RigidBody* generateInstance(glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot);
    void initInstances();
    void removeInstance(unsigned int idx);
    void removeInstance(std::string instanceId);

    unsigned int getIdx(std::string id);

protected:
    bool noTex;
    std::string directory;
    std::vector<Texture> textures_loaded;

    BufferObject modelVBO;
    BufferObject normalModelVBO;

    std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);

    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    Mesh processMesh(Bounds br,
                     unsigned int noVertices, float* vertices,
                     unsigned int noIndices, unsigned int* indices,
                     bool calcTanVectors = true,
                     unsigned int noCollisionPoints = 0, float* collisionPoints = NULL,
                     unsigned int noCollisionFaces = 0, unsigned int* collisionIndices = NULL,
                     bool pad = false);
};


#endif //MARCHING_CUBES_MODEL_H
