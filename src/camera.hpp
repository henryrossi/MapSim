#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
    NORTH,
    SOUTH,
    EAST,
    WEST
};

class Camera {
public:
        virtual glm::mat4 GetViewMatrix() = 0;

        virtual void ProcessKeyboard(CameraMovement direction, float deltaTime) = 0;

        virtual void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) = 0;

        virtual void ProcessMouseScroll(float yoffset) = 0;
};

#endif /* CAMERA_H */