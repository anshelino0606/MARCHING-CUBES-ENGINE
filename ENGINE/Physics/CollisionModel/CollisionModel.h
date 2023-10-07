//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_COLLISIONMODEL_H
#define MARCHING_CUBES_COLLISIONMODEL_H


#include "../CollisionMesh/CollisionMesh.h"
#include "../../Graphics/Model/Model.h"
#include <vector>

class Model;

class CollisionModel {
public:
    CollisionModel(Model* model);

    Model* model;

    std::vector<CollisionMesh> meshes;
};


#endif //MARCHING_CUBES_COLLISIONMODEL_H
