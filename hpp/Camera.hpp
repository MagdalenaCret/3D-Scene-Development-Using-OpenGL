#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> // Required for transformation matrices

namespace gps {

    // Enum for camera movement directions
    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT };

    // Camera class declaration
    class Camera {
    public:
        // Constructor
        Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);

        // Movement methods
        void move(MOVE_DIRECTION direction, float speed);
        void rotate(float pitch, float yaw);
        void rotateAroundY(float angle);
        void scenePreview(float angle);

        // Setters
        void setPosition(glm::vec3 newPosition);
        void setTarget(glm::vec3 newTarget);

        // Getters
        [[nodiscard]] glm::mat4 getViewMatrix() const;
        [[nodiscard]] glm::vec3 getPosition() const;
        [[nodiscard]] glm::vec3 getTarget() const;
        [[nodiscard]] glm::vec3 getFrontDirection() const;
        [[nodiscard]] glm::vec3 getUpDirection() const;

    private:
        // Core vectors defining camera orientation
        glm::vec3 position;    // Camera position
        glm::vec3 target;      // Camera target
        glm::vec3 front;       // Front direction
        glm::vec3 right;       // Right direction
        glm::vec3 up;          // Up direction

        // Original reference vectors
        const glm::vec3 originalUp;
        const glm::vec3 originalFront;

        // Helper methods
        glm::mat4 yawPitchRoll(float yaw, float pitch, float roll); // Custom Yaw-Pitch-Roll matrix generator
        void updateDirectionVectors(); // Updates front, right, and up vectors based on position and target
    };

} // namespace gps

#endif // CAMERA_HPP
