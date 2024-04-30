#include "mapcamera.hpp"

MapCamera::MapCamera(glm::vec3 pos) : Position{pos} {}

MapCamera::MapCamera(float xpos, float ypos, float zpos) : Position{xpos, ypos, zpos} {}

glm::mat4 MapCamera::GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
}

void MapCamera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == NORTH) {
                Position += Up * velocity;
        }
        if (direction == SOUTH) {
                Position -= Up * velocity;
        }
        if (direction == EAST) {
                Position -= Right * velocity;
        }
        if (direction == WEST) {
                Position += Right * velocity;
        }
}

void MapCamera::ProcessMouseScroll(float yoffset){
        FOV -= (float)yoffset;
        if (FOV < 1.0f) {
                FOV = 1.0f;
        }
        if (FOV > 45.0f) {
                FOV = 45.0f;
        }
}