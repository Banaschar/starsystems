#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stdio.h>

#include "global.hpp"
#include "view.hpp"
#include "windowheader.hpp"

#define DEFAULT_NEAR_PLANE 0.1f
#define DEFAULT_FAR_PLANE 2000.0f
const float YAW = 90.0f;
const float PITCH = -30.0f;
const float SPEED = 20.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;
const bool CONSTRAIN_PITCH = true;
const bool AUTO_ROTATE = false;

View::View(GLFWwindow *window, glm::vec3 camPos) : View(window, camPos, glm::vec3(-camPos.x, -camPos.y, -camPos.z)) {}

View::View(GLFWwindow *window, glm::vec3 camPos, glm::vec3 camDir)
    : View(window, camPos, camDir, YAW, PITCH, FOV, SPEED, SENSITIVITY) {}

View::View(GLFWwindow *window, glm::vec3 camPos, glm::vec3 camDir, float yaw, float pitch, float fov, float speed,
           float sensitivity)
    : window_(window), camPosition_(camPos), camDirection_(camDir), yaw_(yaw), pitch_(pitch), fov_(fov), speed_(speed),
      mouseSensitivity_(sensitivity) {

    glfwGetWindowSize(window_, &windowWidth_, &windowHeight_);
    camUp_ = glm::vec3(0, 1, 0);
    worldUp_ = camUp_;
    zoom_ = fov_;
    camPositionOriginal_ = camPosition_;
    cameraSpeed_ = 0.2;
    camRotateVal_ = 0.0;
    autoRotate_ = AUTO_ROTATE;
    constrainPitch_ = CONSTRAIN_PITCH;
    nearPlane_ = DEFAULT_NEAR_PLANE;
    farPlane_ = DEFAULT_FAR_PLANE;

    update();
}

void View::setupInput() {
    glfwSetWindowUserPointer(window_, this);
    mCb = [](GLFWwindow *w, double a, double b) {
        static_cast<View *>(glfwGetWindowUserPointer(w))->mouseCallback(w, a, b);
    };
    sCb = [](GLFWwindow *w, double a, double b) {
        static_cast<View *>(glfwGetWindowUserPointer(w))->scrollCallback(w, a, b);
    };

    glfwSetCursorPosCallback(window_, mCb);
    glfwSetScrollCallback(window_, sCb);
}

glm::mat4 &View::getCameraMatrix() {
    return cameraMatrix_;
}

glm::mat4 &View::getProjectionMatrix() {
    return projectionMatrix_;
}

glm::mat4 View::getOrthoProjection() {
    return glm::ortho(0.0f, (float)windowWidth_, 0.0f, (float)windowHeight_);
}

void View::invertPitch() {
    pitch_ = -pitch_;
}

glm::vec3 &View::getCameraPosition() {
    return camPosition_;
}

glm::vec3 &View::getCameraDirection() {
    return camDirection_;
}

glm::vec3 &View::getWorldNormal() {
    return worldUp_;
}

float View::getNearPlane() {
    return nearPlane_;
}
float View::getFarPlane() {
    return farPlane_;
}

void View::getWindowSize(int *width, int *height) {
    glfwGetWindowSize(window_, width, height);
}

void View::setAutoRotate(bool value) {
    autoRotate_ = value;
}

void View::rotateCamera() {
    camRotateVal_ += cameraSpeed_ * g_deltaTime;

    if (camRotateVal_ > 360)
        camRotateVal_ = camRotateVal_ - 360;

    camPosition_.x = sin(camRotateVal_) * abs(camPositionOriginal_.z);
    camPosition_.z = cos(camRotateVal_) * abs(camPositionOriginal_.z);

    cameraMatrix_ = glm::lookAt(camPosition_, camDirection_, camUp_);
}

void View::updateForce() {
    flagUpdate_ = true;
    update_();
}

void View::update() {
    processInputs();
    update_();
}

void View::update_() {
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

        projectionMatrix_ =
            glm::perspective(glm::radians(zoom_), (float)windowWidth_ / (float)windowHeight_, nearPlane_, farPlane_);
        cameraMatrix_ = glm::lookAt(camPosition_, camPosition_ + camDirection_, camUp_);

        flagUpdate_ = false;
    }
}

void View::processInputs() {
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        camPosition_ += camDirection_ * g_deltaTime * speed_;
        flagUpdate_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        camPosition_ -= camDirection_ * g_deltaTime * speed_;
        flagUpdate_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        camPosition_ += camRight_ * g_deltaTime * speed_;
        flagUpdate_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        camPosition_ -= camRight_ * g_deltaTime * speed_;
        flagUpdate_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_F) == GLFW_PRESS) {
        camPosition_ -= glm::vec3(0, 1, 0) * g_deltaTime * speed_;
        flagUpdate_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_R) == GLFW_PRESS) {
        camPosition_ += glm::vec3(0, 1, 0) * g_deltaTime * speed_;
        flagUpdate_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_SLASH) == GLFW_PRESS) {
        speed_ = speed_ <= 2 ? 1 : speed_ - 1;
    }

    if (glfwGetKey(window_, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
        speed_ = speed_ >= 50 ? 50 : speed_ + 1;
    }
}

void View::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouseMove_) {
        lastX_ = xpos;
        lastY_ = ypos;
        firstMouseMove_ = false;
    }

    float xoffset = xpos - lastX_;
    float yoffset = lastY_ - ypos; // reversed

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
    zoom_ -= (float)yoffset;
    if (zoom_ < 1.0f)
        zoom_ = 1.0f;
    if (zoom_ > fov_)
        zoom_ = fov_;

    flagUpdate_ = true;
}