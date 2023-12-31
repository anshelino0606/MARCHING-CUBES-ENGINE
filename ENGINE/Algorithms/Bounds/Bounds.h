//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_BOUNDS_H
#define MARCHING_CUBES_BOUNDS_H


#include <glm/glm.hpp>
#include "../../Physics/RigidBody/RigidBody.h"

namespace Octree {
    class node;
}

class CollisionMesh;

enum class BoundTypes : unsigned char {
    AABB    = 0x00,
    SPHERE  = 0x01
};

class Bounds {
public:
    Bounds() = default;
    // constructor with type
    Bounds(BoundTypes type);

    // construct as sphere
    Bounds(glm::vec3 center, float radius);

    // construct as AABB
    Bounds(glm::vec3 min, glm::vec3 max);

    BoundTypes type;
    RigidBody* instance;
    CollisionMesh* collisionMesh;
    Octree::node* cell;

    glm::vec3 center;
    float radius;

    glm::vec3 ogCenter;
    float ogRadius;

    glm::vec3 min;
    glm::vec3 max;

    glm::vec3 ogMin;
    glm::vec3 ogMax;

    void transform();

    glm::vec3 calculateCenter();
    glm::vec3 calculateDimensions();


    bool containsPoint(glm::vec3 pt);
    bool containsRegion(Bounds br);
    bool intersectsWith(Bounds br);

    bool operator==(Bounds br);
};

//namespace Octree {
//    class node;
//}




#endif //MARCHING_CUBES_BOUNDS_H
