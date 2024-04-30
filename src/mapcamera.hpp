#ifndef MAPCAMERA_H
#define MAPCAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
    NORTH,
    SOUTH,
    EAST,
    WEST
};

const float default_speed =  2.5f;
const float default_fov =  45.0f;


class MapCamera {
public:
        glm::vec3 Position;
        glm::vec3 Front{0.0f, 0.0f, -1.0f};
        glm::vec3 Up{0.0f, 1.0f, 0.0f};
        glm::vec3 Right{1.0f, 0.0f, 0.0f};
        glm::vec3 WorldUp{0.0f, 1.0f, 0.0f};
        float MovementSpeed{default_speed};
        float FOV{default_fov};

        MapCamera(glm::vec3 pos);
        MapCamera(float xpos, float ypos, float zpos);

        glm::mat4 GetViewMatrix();

        void ProcessKeyboard(CameraMovement direction, float deltaTime);

        void ProcessMouseScroll(float yoffset);
};

#endif /* MAPCAMERA_H */