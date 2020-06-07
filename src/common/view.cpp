#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "view.hpp"
#include "global.hpp"

View::View(GLFWwindow *window, glm::vec3 camPos) : 
            View(window, camPos, glm::vec3(0,0,0)) {}

View::View(GLFWwindow *window, glm::vec3 camPos, glm::vec3 camDir) :
            View(window, camPos, camDir, 3.14f, 0.0f, 45.0f, 3.0f, 0.005f) {}

View::View(GLFWwindow *window, glm::vec3 camPos, glm::vec3 camDir, float angleH, float angleV, float fov, float speed, 
                            float mouseSpeed) : window_(window), camPosition_(camPos), camDirection_(camDir), 
                            horizontalAngle_(angleH), verticalAngle_(angleV), 
                            fov_(fov), speed_(speed), mouseSpeed_(mouseSpeed) {
    glfwGetWindowSize(window_, &windowWidth_, &windowHeight_);
    //fprintf(stdout, "width: %i, height: %i\n", windowWidth, windowHeight);
    camUp_ = glm::vec3(0,1,0);
    // projection matrix
    projectionMatrix_ = glm::perspective(glm::radians(fov_), 16.0f / 9.0f, 0.1f, 100.0f);

    // camera matrix
    cameraMatrix_ = glm::lookAt(
        camPosition_,
        camDirection_,
        camUp_);

    camPositionOriginal_ = camPosition_;
    cameraSpeed_ = 1.0;
    camRotateVal_ = 0.0;
}

glm::mat4 View::getCameraMatrix() {
    return cameraMatrix_;
}

glm::mat4 View::getProjectionMatrix() {
    return projectionMatrix_;
}

glm::vec3 View::getCameraPosition() {
    return camPosition_;
}

void View::rotateCamera() {
    camRotateVal_ += cameraSpeed_ * deltaTime;

    if (camRotateVal_ > 360)
        camRotateVal_ = camRotateVal_ - 360;

    camPosition_.x = sin(camRotateVal_) * abs(camPositionOriginal_.z);
    camPosition_.z = cos(camRotateVal_) * abs(camPositionOriginal_.z);
    
    cameraMatrix_ = glm::lookAt(
    camPosition_,
    camDirection_,
    camUp_);
}

void View::updateFromInputs() {
    static double lastTime = glfwGetTime();

    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    double xpos, ypos;
    glfwGetCursorPos(window_, &xpos, &ypos);

    glfwSetCursorPos(window_, windowWidth_/2, windowHeight_/2);

    horizontalAngle_ += mouseSpeed_ * float(windowWidth_/2 - xpos);
    verticalAngle_ += mouseSpeed_ * float(windowHeight_/2 - ypos);

    camDirection_ = glm::vec3(
        cos(verticalAngle_) * sin(horizontalAngle_),
        sin(verticalAngle_),
        cos(verticalAngle_) * cos(horizontalAngle_));

    glm::vec3 right = glm::vec3(
        sin(horizontalAngle_ - 3.14f/2.0f),
        0,
        cos(horizontalAngle_ - 3.14f/2.0f));

    camUp_ = glm::cross(right, camDirection_);
    
    // Move forward
    if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS){
        camPosition_ += camDirection_ * deltaTime * speed_;
    }
    // Move backward
    if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS){
        camPosition_ -= camDirection_ * deltaTime * speed_;
    }
    // Strafe right
    if (glfwGetKey(window_, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        camPosition_ += right * deltaTime * speed_;
    }
    // Strafe left
    if (glfwGetKey(window_, GLFW_KEY_LEFT ) == GLFW_PRESS){
        camPosition_ -= right * deltaTime * speed_;
    }

    cameraMatrix_ = glm::lookAt(
        camPosition_, camPosition_+ camDirection_, camUp_);

    lastTime = currentTime;
}