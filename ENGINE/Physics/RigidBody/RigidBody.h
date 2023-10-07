//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_RIGIDBODY_H
#define MARCHING_CUBES_RIGIDBODY_H


#include <glm/glm.hpp>
#include <string>

#define INSTANCE_DEAD (unsigned char)0b00000001
#define INSTANCE_MOVED (unsigned char)0b00000010

#define COLLISION_THRESHOLD 0.05f

class RigidBody {
public:
    RigidBody(std::string modelId = "",
              glm::vec3 size = glm::vec3(1.0f),
              float mass = 1.0f,
              glm::vec3 pos = glm::vec3(0.0f),
              glm::vec3 rot = glm::vec3(0.0f));

    unsigned char state;
    float mass;

    glm::vec3 pos;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    glm::vec3 size;
    glm::vec3 rot;

    glm::mat4 model;
    glm::mat3 normalModel;

    std::string modelId;
    std::string instanceId;

    float lastCollision;
    std::string lastCollisionID;

    bool operator==(RigidBody rb);
    bool operator==(std::string id);

    void update(float dt);

    void applyForce(glm::vec3 force);
    void applyForce(glm::vec3 direction, float magnitude);

    void applyAcceleration(glm::vec3 acceleration);
    void applyAcceleration(glm::vec3 direction, float magnitude);

    void applyImpulse(glm::vec3 force, float dt);
    void applyImpulse(glm::vec3 direction, float magnitude, float dt);

    void transferEnergy(float joules, glm::vec3 direction);

    void handleCollision(RigidBody* inst, glm::vec3 norm);
};


#endif //MARCHING_CUBES_RIGIDBODY_H
