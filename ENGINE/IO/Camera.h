//
// Created by Anhelina Modenko on 02.07.2023.
//

#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraDirection {
    NONE = 0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:

    static Camera defaultCamera;

    Camera(glm::vec3 position = glm::vec3(0.0f));

    void updateCameraDirection(double dx, double dy);
    void updateCameraPos(CameraDirection dir, double dt);
    void updateCameraZoom(double dy);

    float getZoom();

    glm::mat4 getViewMatrix();
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;

    glm::vec3 worldUp;
    float zoom;
private:
    void updateCameraVectors();

    float yaw;
    float pitch;
    float speed;

};


#endif //OPENGL_CAMERA_H
