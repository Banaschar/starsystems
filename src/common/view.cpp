#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>

#include "view.hpp"
#include "global.hpp"

// default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;
const bool CONSTRAIN_PITCH = true;
const bool AUTO_ROTATE = false;

View::View(GLFWwindow *window, glm::vec3 camPos) : 
            View(window, camPos, glm::vec3(0,0,0)) {}

View::View(GLFWwindow *window, glm::vec3 camPos, glm::vec3 camDir) :
            View(window, camPos, camDir, YAW, PITCH, FOV, SPEED, SENSITIVITY) {}

View::View(GLFWwindow *window, glm::vec3 camPos, glm::vec3 camDir, float yaw, float pitch, 
            float fov, float speed, float sensitivity) : 
                            window_(window), camPosition_(camPos), camDirection_(camDir), 
                            yaw_(yaw), pitch_(pitch), 
                            fov_(fov), speed_(speed), mouseSensitivity_(sensitivity) {
    
    glfwGetWindowSize(window_, &windowWidth_, &windowHeight_);
    camUp_ = glm::vec3(0,1,0);
    worldUp_ = camUp_;
    zoom_ = fov_;
    camPositionOriginal_ = camPosition_;
    cameraSpeed_ = 0.2;
    camRotateVal_ = 0.0;
    autoRotate_ = AUTO_ROTATE;
    constrainPitch_ = CONSTRAIN_PITCH;

    update();
}

void View::setupInput() {
    glfwSetWindowUserPointer(window_, this);
    mCb = [](GLFWwindow *w, double a, double b) {
        static_cast<View*>(glfwGetWindowUserPointer(w))->mouseCallback(w, a, b);
    };
    sCb = [](GLFWwindow *w, double a, double b) {
        static_cast<View*>(glfwGetWindowUserPointer(w))->scrollCallback(w, a, b);
    };
    
    glfwSetCursorPosCallback(window_, mCb);
    glfwSetScrollCallback(window_, sCb);
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

void View::setAutoRotate(bool value) {
    autoRotate_ = value;
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

void View::update() {
    processInputs();

    if (flagUpdate_) {
        // direction vector
        glm::vec3 dir;
        dir.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        dir.y = sin(glm::radians(pitch_));
        dir.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        camDirection_ = glm::normalize(dir);
        // right and up
        camRight_ = glm::normalize(glm::cross(camDirection_, worldUp_));
        camUp_ = glm::normalize(glm::cross(camRight_, camDirection_));

        projectionMatrix_ = glm::perspective(glm::radians(zoom_), 
                            (float)windowWidth_ / (float)windowHeight_, 0.1f, 200.0f);
        cameraMatrix_ = glm::lookAt(camPosition_, camPosition_ + camDirection_, camUp_);

        flagUpdate_ = false;
    }
}

void View::processInputs() {    
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS){
        camPosition_ += camDirection_ * deltaTime * speed_;
        flagUpdate_ = true;
    }
    
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS){
        camPosition_ -= camDirection_ * deltaTime * speed_;
        flagUpdate_ = true;
    }
    
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS){
        camPosition_ += camRight_ * deltaTime * speed_;
        flagUpdate_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS){
        camPosition_ -= camRight_ * deltaTime * speed_;
        flagUpdate_ = true;
    }
}

void View::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouseMove_) {
        lastX_ = xpos;
        lastY_ = ypos;
        firstMouseMove_ = false;
    }

    float xoffset = xpos - lastX_;
    float yoffset = lastY_ - ypos; //reversed

    lastX_ = xpos;
    lastY_ = ypos;

    xoffset *= mouseSensitivity_;
    yoffset *= mouseSensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    if (constrainPitch_) {
        if (pitch_ > 89.0f)
            pitch_ = 89.0f;
        if (pitch_ < -89.0f)
            pitch_ = -89.0f;
    }

    flagUpdate_ = true;
}

void View::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    zoom_ -= (float) yoffset;
    if (zoom_ < 1.0f)
        zoom_ = 1.0f;
    if (zoom_ > fov_)
        zoom_ = fov_;

    flagUpdate_ = true;
}