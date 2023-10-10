#ifndef MARCHING_CUBES_SPHERE_HPP
#define MARCHING_CUBES_SPHERE_HPP

#include "../Graphics/Model/Model.h"

class Sphere : public Model {
public:
    Sphere(unsigned int maxNoInstances)
            : Model("sphere", maxNoInstances, NO_TEX | DYNAMIC) {}

    void init() {
        loadModel("/Users/anhelinamodenko/CLionProjects/MARCHING-CUBES/ASSETS/Models/sphere/scene.gltf");
    }
};

#endif