#include "flycamera.hpp"

FlyCamera::FlyCamera(glm::vec3 position, float screen_w, float screen_h) 
                    : Position{position}, lastX{screen_w / 2}, lastY{screen_h / 2} {
        updateCameraVectors();
}

FlyCamera::FlyCamera(float posX, float posY, float posZ, float screen_w, float screen_h)
                    : Position{posX, posY, posZ}, lastX{screen_w / 2}, lastY{screen_h / 2} {
        updateCameraVectors();
}

glm::mat4 FlyCamera::GetViewMatrix(){
        return glm::lookAt(Position, Position + Front, Up);
}

void FlyCamera::ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == NORTH)
            Position += Front * velocity;
        if (direction == SOUTH)
            Position -= Front * velocity;
        if (direction == EAST)
            Position -= Right * velocity;
        if (direction == WEST)
            Position += Right * velocity;
    }


void FlyCamera::ProcessMouseMovement(float xpos, float ypos, GLboolean constrainPitch) {
        if (firstMouse) {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
}

    
void FlyCamera::ProcessMouseScroll(float yoffset){
        FOV -= (float)yoffset;
        if (FOV < 1.0f)
            FOV = 1.0f;
        if (FOV > 45.0f)
            FOV = 45.0f;
}

void FlyCamera::updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
}