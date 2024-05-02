#ifndef MAPCAMERA_H
#define MAPCAMERA_H

#include "camera.hpp"

class MapCamera : public Camera {
public:
        glm::vec3 Position;
        glm::vec3 Front{0.0f, 0.0f, -1.0f};
        glm::vec3 Up{0.0f, 1.0f, 0.0f};
        glm::vec3 Right{1.0f, 0.0f, 0.0f};
        glm::vec3 WorldUp{0.0f, 1.0f, 0.0f};
        float MovementSpeed{2.5f};
        float FOV{45.0f};

        MapCamera(glm::vec3 pos);
        MapCamera(float xpos, float ypos, float zpos);

        glm::mat4 GetViewMatrix();

        void ProcessKeyboard(CameraMovement direction, float deltaTime);

        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {};

        void ProcessMouseScroll(float yoffset);
};

#endif /* MAPCAMERA_H */