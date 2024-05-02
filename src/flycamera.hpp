#ifndef FLYCAMERA_H
#define FLYCAMERA_H

#include "camera.hpp"

class FlyCamera : public Camera {
public:
        glm::vec3 Position;
        glm::vec3 Front{0.0f, 0.0f, -1.0f};
        glm::vec3 Up{0.0f, 1.0f, 0.0f};
        glm::vec3 Right{1.0f, 0.0f, 0.0f};
        glm::vec3 WorldUp{0.0f, 1.0f, 0.0f};
        float Yaw{-90.0f};
        float Pitch{0.0f};
        float MovementSpeed{2.5f};
        float MouseSensitivity{0.1f};
        float FOV{45.0f};

        float lastX;
        float lastY;
        bool firstMouse = true;


        FlyCamera(glm::vec3 pos, float screen_w, float screen_h);
        FlyCamera(float xpos, float ypos, float zpos, float screen_w, float screen_h);

        glm::mat4 GetViewMatrix();

        void ProcessKeyboard(CameraMovement direction, float deltaTime);

        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

        void ProcessMouseScroll(float yoffset);
private:
        void updateCameraVectors();
};

#endif /* FLYCAMERA_H */
