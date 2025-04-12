#include "Camera.hpp"

namespace gps {

    Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
        : position(position), target(target), originalUp(up), originalFront(glm::normalize(target - position)) {
        updateDirectionVectors();
    }

    void Camera::move(MOVE_DIRECTION direction, float speed) {
        switch (direction) {
        case MOVE_FORWARD:
            position += speed * front;
            break;
        case MOVE_BACKWARD:
            position -= speed * front;
            break;
        case MOVE_LEFT:
            position -= speed * right;
            break;
        case MOVE_RIGHT:
            position += speed * right;
            break;
        }
        target = position + front;
    }

    glm::mat4 Camera::yawPitchRoll(float yaw, float pitch, float roll) {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
        rotation = glm::rotate(rotation, pitch, glm::vec3(1.0f, 0.0f, 0.0f));               // Pitch
        rotation = glm::rotate(rotation, roll, glm::vec3(0.0f, 0.0f, 1.0f));               // Roll
        return rotation;
    }

    void Camera::rotate(float pitch, float yaw) {
        glm::mat4 rotation = yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);

        front = glm::normalize(glm::vec3(rotation * glm::vec4(originalFront, 0.0f)));
        right = glm::normalize(glm::cross(front, originalUp));
        up = glm::cross(right, front);
        target = position + front;
    }

    void Camera::rotateAroundY(float angle) {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        position = glm::vec3(rotation * glm::vec4(position, 1.0f));
        updateDirectionVectors();
    }

    glm::mat4 Camera::getViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    void Camera::setPosition(glm::vec3 newPosition) {
        position = newPosition;
        updateDirectionVectors();
    }

    void Camera::setTarget(glm::vec3 newTarget) {
        target = newTarget;
        updateDirectionVectors();
    }

    void Camera::scenePreview(float angle) {
        setPosition(glm::vec3(20, 5.3, 1.62146f));
        rotateAroundY(angle);
    }

    void Camera::updateDirectionVectors() {
        front = glm::normalize(target - position);
        right = glm::normalize(glm::cross(front, originalUp));
        up = glm::cross(right, front);
    }

    glm::vec3 gps::Camera::getPosition() const {
        return position;
    }

} // namespace gps
