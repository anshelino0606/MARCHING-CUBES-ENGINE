//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_BOUNDS_H
#define MARCHING_CUBES_BOUNDS_H


#include <glm/glm.hpp>

#include "../../Physics/RigidBody/RigidBody.h"
#include "../Octree/Octree.h"
#include "../../Physics/CollisionMesh/CollisionMesh.h"

// forward declaration
namespace Octree {
    class node;
}
class CollisionMesh;

enum class BoundTypes : unsigned char {
    AABB    = 0x00,	// 0x00 = 0	// Axis-aligned bounding box
    SPHERE  = 0x01	// 0x01 = 1
};

/*
    class to represent bounding region
*/

class Bounds {
public:
    // constructor with type
    Bounds(BoundTypes type = BoundTypes::AABB);

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


#endif //MARCHING_CUBES_BOUNDS_H
