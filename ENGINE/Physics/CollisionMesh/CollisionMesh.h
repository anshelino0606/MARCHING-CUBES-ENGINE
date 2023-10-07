//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_COLLISIONMESH_H
#define MARCHING_CUBES_COLLISIONMESH_H


#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../../Algorithms/Bounds/Bounds.h"

class CollisionModel;
class CollisionMesh;
class RigidBody;

struct Face {
    CollisionMesh* mesh;
    unsigned int i1, i2, i3;

    glm::vec3 baseNormal;
    glm::vec3 norm;

    bool collidesWithFace(RigidBody* thisRB, struct Face& face, RigidBody* faceRB, glm::vec3& retNorm);
    bool collidesWithSphere(RigidBody* thisRB, Bounds& br, glm::vec3& retNorm);
};

class CollisionMesh {
public:
    CollisionModel* model;
    Bounds br;

    std::vector<glm::vec3> points;
    std::vector<Face> faces;

    CollisionMesh(unsigned int noPoints, float* coordinates, unsigned int noFaces, unsigned int* indices);
};


#endif //MARCHING_CUBES_COLLISIONMESH_H
