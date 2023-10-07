//
// Created by Anhelina Modenko on 06.10.2023.
//

#include "Octree.h"
#include "../AVL/Avl.h"
#include "../../Models/box.hpp"

Octree::node::node()
        : region(BoundTypes::AABB) {}
        
Octree::node::node(Bounds bounds)
        : region(bounds) {}
        
Octree::node::node(Bounds bounds, std::vector<Bounds> objectList)
        : region(bounds) {
    objects.insert(objects.end(), objectList.begin(), objectList.end());
}


void Octree::calculateBounds(Bounds &out, Octant octant, Bounds parentRegion) {
    glm::vec3 center = parentRegion.calculateCenter();
    if (octant == Octant::O1)
        out = Bounds(center, parentRegion.max);
    else if (octant == Octant::O2)
        out = Bounds(glm::vec3(parentRegion.min.x, center.y, center.z), glm::vec3(center.x, parentRegion.max.y, parentRegion.max.z));
    else if (octant == Octant::O3) 
        out = Bounds(glm::vec3(parentRegion.min.x, parentRegion.min.y, center.z), glm::vec3(center.x, center.y, parentRegion.max.z));
    else if (octant == Octant::O4)
        out = Bounds(glm::vec3(center.x, parentRegion.min.y, center.z), glm::vec3(parentRegion.max.x, center.y, parentRegion.max.z));
    else if (octant == Octant::O5)
        out = Bounds(glm::vec3(center.x, center.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, parentRegion.max.y, center.z));
    else if (octant == Octant::O6)
        out = Bounds(glm::vec3(parentRegion.min.x, center.y, parentRegion.min.z), glm::vec3(center.x, parentRegion.max.y, center.z));
    else if (octant == Octant::O7)
        out = Bounds(parentRegion.min, center);
    else if (octant == Octant::O8)
        out = Bounds(glm::vec3(center.x, parentRegion.min.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, center.y, center.z));
}

void Octree::node::addToPending(RigidBody* instance, Model *model) {
    for (Bounds br : model->boundingRegions) {
        br.instance = instance;
        br.transform();
        queue.push(br);
    }
}

void Octree::node::build() {
    Bounds octants[NO_CHILDREN];
    glm::vec3 dimensions = region.calculateDimensions();
    std::vector<Bounds> octLists[NO_CHILDREN];

    if (objects.size() <= 1) {
        goto setVars;
    }

    for (int i = 0; i < 3; i++) {
        if (dimensions[i] < MIN_BOUNDS) {
            goto setVars;
        }
    }

    for (int i = 0; i < NO_CHILDREN; i++) {
        calculateBounds(octants[i], (Octant)(1 << i), region);
    }

    for (int i = 0, len = objects.size(); i < len; i++) {
        Bounds br = objects[i];
        for (int j = 0; j < NO_CHILDREN; j++) {
            if (octants[j].containsRegion(br)) {
                octLists[j].push_back(br);
                objects.erase(objects.begin() + i);
                i--;
                len--;
                break;
            }
        }
    }

    for (int i = 0; i < NO_CHILDREN; i++) {
        if (octLists[i].size() != 0) {
            children[i] = new node(octants[i], octLists[i]);
            States::activateIndex(&activeOctants, i);
            children[i]->parent = this;
            children[i]->build();
        }
    }

    setVars:
    treeBuilt = true;
    treeReady = true;

    for (int i = 0; i < objects.size(); i++) {
        objects[i].cell = this;
    }
}

void Octree::node::update(Box &box) {
    if (treeBuilt && treeReady) {
        box.positions.push_back(region.calculateCenter());
        box.sizes.push_back(region.calculateDimensions());

        if (objects.size() == 0) {
            if (!activeOctants) {
                if (currentLifespan == -1) {
                    currentLifespan = maxLifespan;
                }
                else if (currentLifespan > 0) {
                    currentLifespan--;
                }
            }
        }
        else {
            if (currentLifespan != -1) {
                if (maxLifespan <= 64) {
                    maxLifespan <<= 2;
                }
            }
        }

        for (int i = 0, listSize = objects.size(); i < listSize; i++) {
            if (States::active(&objects[i].instance->state, INSTANCE_DEAD)) {
                objects.erase(objects.begin() + i);
                i--;
                listSize--;
            }
        }

        std::stack<int> movedObjects;
        for (int i = 0, listSize = objects.size(); i < listSize; i++) {
            if (States::active(&objects[i].instance->state, INSTANCE_MOVED)) {
                objects[i].transform();
                movedObjects.push(i);
            }
            box.positions.push_back(objects[i].calculateCenter());
            box.sizes.push_back(objects[i].calculateDimensions());
        }

        unsigned char flags = activeOctants;
        for (int i = 0;
             flags > 0;
             flags >>= 1, i++) {
            if (States::indexActive(&flags, 0) && children[i]->currentLifespan == 0) {
                if (children[i]->objects.size() > 0) {
                    children[i]->currentLifespan = -1;
                }
                else {
                    free(children[i]);
                    children[i] = nullptr;
                    States::deactivateIndex(&activeOctants, i);
                }
            }
        }

        for (unsigned char flags = activeOctants, i = 0;
             flags > 0;
             flags >>= 1, i++) {
            if (States::indexActive(&flags, 0)) {
                if (children[i] != nullptr) {
                    children[i]->update(box);
                }
            }
        }


        Bounds movedObj;
        while (movedObjects.size() != 0) {
            movedObj = objects[movedObjects.top()]; // top of stack
            node* current = this; // start at this node

            while (!current->region.containsRegion(movedObj)) {
                if (current->parent != nullptr) {
                    current = current->parent;
                }
                else {
                    break;
                }
            }
            objects.erase(objects.begin() + movedObjects.top());
            movedObjects.pop();
            current->queue.push(movedObj);

            current = movedObj.cell;
            current->checkCollisionsSelf(movedObj);
            current->checkCollisionsChildren(movedObj);
            while (current->parent) {
                current = current->parent;
                current->checkCollisionsSelf(movedObj);
            }
        }
    }

    processPending();
}

void Octree::node::processPending() {
    if (!treeBuilt) {
        while (queue.size() != 0) {
            objects.push_back(queue.front());
            queue.pop();
        }
        build();
    }
    else {
        for (int i = 0, len = queue.size(); i < len; i++) {
            Bounds br = queue.front();
            if (region.containsRegion(br)) {
                insert(br);
            }
            else {
                br.transform();
                queue.push(br);
            }
            queue.pop();
        }
    }
}

bool Octree::node::insert(Bounds obj) {

    glm::vec3 dimensions = region.calculateDimensions();
    if (objects.size() == 0 ||
        dimensions.x < MIN_BOUNDS ||
        dimensions.y < MIN_BOUNDS ||
        dimensions.z < MIN_BOUNDS
            ) {
        obj.cell = this;
        objects.push_back(obj);
        return true;
    }

    if (!region.containsRegion(obj)) {
        return parent == nullptr ? false : parent->insert(obj);
    }

    Bounds octants[NO_CHILDREN];
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (children[i] != nullptr) {
            octants[i] = children[i]->region;
        }
        else {
            calculateBounds(octants[i], (Octant)(1 << i), region);
        }
    }

    objects.push_back(obj);

    std::vector<Bounds> octLists[NO_CHILDREN]; // octants with objects
    for (int i = 0, len = objects.size(); i < len; i++) {
        objects[i].cell = this;
        for (int j = 0; j < NO_CHILDREN; j++) {
            if (octants[j].containsRegion(objects[i])) {
                octLists[j].push_back(objects[i]);
                objects.erase(objects.begin() + i);
                i--;
                len--;
                break;
            }
        }
    }

    for (int i = 0; i < NO_CHILDREN; i++) {
        if (octLists[i].size() != 0) {
            if (children[i]) {
                for (Bounds br : octLists[i]) {
                    children[i]->insert(br);
                }
            }
            else {
                children[i] = new node(octants[i], octLists[i]);
                children[i]->parent = this;
                States::activateIndex(&activeOctants, i);
                children[i]->build();
            }
        }
    }

    return true;
}

void Octree::node::checkCollisionsSelf(Bounds obj) {
    for (Bounds br : objects) {
        if (br.instance->instanceId == obj.instance->instanceId) {
            continue;
        }

        if (br.intersectsWith(obj)) {
            unsigned int noFacesBr = br.collisionMesh ? br.collisionMesh->faces.size() : 0;
            unsigned int noFacesObj = obj.collisionMesh ? obj.collisionMesh->faces.size() : 0;

            glm::vec3 norm;

            if (noFacesBr) {
                unsigned int noFacesBr = br.collisionMesh->faces.size();

                if (noFacesObj) {
                    unsigned int noFacesObj = obj.collisionMesh->faces.size();

                    for (unsigned int i = 0; i < noFacesBr; i++) {
                        for (unsigned int j = 0; j < noFacesObj; j++) {
                            if (br.collisionMesh->faces[i].collidesWithFace(
                                    br.instance,
                                    obj.collisionMesh->faces[j],
                                    obj.instance,
                                    norm
                            )) {
                                std::cout << "1.INSTANCE:" << br.instance->instanceId
                                          << " (" << br.instance->modelId << ") COLLIDES WITH INSTANCE " << obj.instance->instanceId 
                                          << " (" << obj.instance->modelId << ")" << std::endl;

                                obj.instance->handleCollision(br.instance, norm);

                                break;
                            }
                        }
                    }
                }
                else {
                    for (unsigned int i = 0; i < noFacesBr; i++) {
                        if (br.collisionMesh->faces[i].collidesWithSphere(
                                br.instance,
                                obj,
                                norm
                        )) {
                            std::cout << "2.INSTANCE " << br.instance->instanceId
                                      << " (" << br.instance->modelId << ") COLLIDES WITH INSTANCE " << obj.instance->instanceId 
                                      << " (" << obj.instance->modelId << ")" << std::endl;

                            obj.instance->handleCollision(br.instance, norm);

                            break;
                        }
                    }
                }
            }
            else {
                if (noFacesObj) {
                    unsigned int noFacesObj = obj.collisionMesh->faces.size();

                    for (int i = 0; i < noFacesObj; i++) {
                        if (obj.collisionMesh->faces[i].collidesWithSphere(
                                obj.instance,
                                br,
                                norm
                        )) {
                            std::cout << "3. INSTANCE " << br.instance->instanceId
                                      << " (" << br.instance->modelId << ") COLLIDES WITH INSTANCE " << obj.instance->instanceId 
                                      << " (" << obj.instance->modelId << ")" << std::endl;
                            obj.instance->handleCollision(br.instance, norm);
                            break;
                        }
                    }
                }
                else {
                    std::cout << "4. INSTANCE " << br.instance->instanceId
                              << " (" << br.instance->modelId << ") COLLIDES WITH INSTANCE " << obj.instance->instanceId 
                              << " (" << obj.instance->modelId << ")" << std::endl;

                    norm = obj.center - br.center;
                    obj.instance->handleCollision(br.instance, norm);
                }
            }
        }
    }
}

void Octree::node::checkCollisionsChildren(Bounds obj) {

    for (int flags = activeOctants, i = 0;
         flags > 0;
         flags >>= 1, i++) {
        if (States::indexActive(&flags, 0) && children[i]) {
            children[i]->checkCollisionsSelf(obj);
            children[i]->checkCollisionsChildren(obj);
        }
    }

}

void Octree::node::destroy() {

    for (int flags = activeOctants, i = 0;
         flags > 0;
         flags >>= 1, i++) {
        if (States::indexActive(&flags, 0)) {
            if (children[i] != nullptr) {
                children[i]->destroy();
                children[i] = nullptr;
            }
        }

    }

    objects.clear();
    while (queue.size() != 0) {
        queue.pop();
    }
}