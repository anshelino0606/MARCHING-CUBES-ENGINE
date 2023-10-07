//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_OCTREE_H
#define MARCHING_CUBES_OCTREE_H


#define NO_CHILDREN 8
#define MIN_BOUNDS 0.5

#include <vector>
#include <queue>
#include <stack>

#include "../List/List.h"
#include "../States/States.h"
#include "../Bounds/Bounds.h"
#include "../../Models/box.hpp"

#include "../../Graphics/Model/Model.h"

namespace Octree {
    /*
        enum to represent octants
    */

    enum class Octant : unsigned char {
        O1 = 0x01,
        O2 = 0x02,
        O3 = 0x04,
        O4 = 0x08,
        O5 = 0x10,
        O6 = 0x20,
        O7 = 0x40,
        O8 = 0x80
    };

    void calculateBounds(Bounds &out, Octant octant, Bounds parentRegion);

    class node {
    public:
        node();
        node(Bounds bounds);
        node(Bounds bounds, std::vector<Bounds> objectList);

        node* parent;
        node* children[NO_CHILDREN];

        unsigned char activeOctants;

        bool treeReady = false;
        bool treeBuilt = false;

        short maxLifespan = 8;
        short currentLifespan = -1;

        std::vector<Bounds> objects;
        std::queue<Bounds> queue;

        Bounds region;

        void addToPending(RigidBody* instance, Model *model);
        void build();
        void update(Box &box);
        void processPending();
        bool insert(Bounds obj);

        void checkCollisionsSelf(Bounds obj);
        void checkCollisionsChildren(Bounds obj);

        void destroy();
    };
}



#endif //MARCHING_CUBES_OCTREE_H
