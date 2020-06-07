#ifndef VIEW_H
#define VIEW_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class View {
public:
    View(GLFWwindow *window, glm::vec3 camPos);
    View(GLFWwindow *window, glm::vec3 camPos, glm::vec3 camDir);
    View(GLFWwindow *window, glm::vec3 camPos, glm::vec3 camDir, float angleH, float angleVert, float fov,
        float speed, float mouseSpeed);
    glm::mat4 getCameraMatrix();
    glm::mat4 getProjectionMatrix();
    glm::vec3 getCameraPosition();
    void updateFromInputs();
    void rotateCamera();
private:
    GLFWwindow *window_;
    int windowWidth_, windowHeight_;
    glm::mat4 cameraMatrix_;
    glm::mat4 projectionMatrix_;
    glm::vec3 camPosition_;
    glm::vec3 camDirection_;
    glm::vec3 camUp_;
    glm::vec3 camPositionOriginal_;
    float horizontalAngle_;
    float verticalAngle_;
    float fov_;
    float speed_;
    float mouseSpeed_;
    float cameraSpeed_;
    float camRotateVal_;
};
#endif