#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stdio.h>

#include "global.hpp"
#include "view.hpp"
#include "windowheader.hpp"

#define DEFAULT_NEAR_PLANE 1.1f
#define DEFAULT_FAR_PLANE 10000.0f
const float PI = 3.14159265359;
const float YAW = 90.0f;
const float PITCH = -30.0f; //-30.0f;
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
    nearPlaneDistance_ = DEFAULT_NEAR_PLANE;
    farPlaneDistance_ = DEFAULT_FAR_PLANE;

    updateForce();

    /* Setup frustum planes */
    float angle = PI / 2.0f + fov_ / 2.0f + 0.1f;
    farPlane_.normal = -camDirection_;
    nearPlane_.normal = camDirection_;
    leftPlane_.normal = glm::vec3(glm::rotate(glm::mat4(), angle, camUp_) * glm::vec4(camDirection_, 1.0f));
    rightPlane_.normal = glm::vec3(glm::rotate(glm::mat4(), -angle, camUp_) * glm::vec4(camDirection_, 1.0f));
    topPlane_.normal = glm::vec3(glm::rotate(glm::mat4(), angle, camRight_) * glm::vec4(camDirection_, 1.0f));
    bottomPlane_.normal = glm::vec3(glm::rotate(glm::mat4(), -angle, camRight_) * glm::vec4(camDirection_, 1.0f));

    frustumPlanes_.insert(frustumPlanes_.begin(), {&farPlane_, &nearPlane_, &leftPlane_, &rightPlane_, &topPlane_, &bottomPlane_});
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
    return nearPlaneDistance_;
}
float View::getFarPlane() {
    return farPlaneDistance_;
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
            glm::perspective(glm::radians(zoom_), (float)windowWidth_ / (float)windowHeight_, nearPlaneDistance_, farPlaneDistance_);
        cameraMatrix_ = glm::lookAt(camPosition_, camPosition_ + camDirection_, camUp_);

        /* Update frustum planes */
        nearPlane_.point = camPosition_ - dir * nearPlaneDistance_;
        farPlane_.point = camPosition_ - dir * farPlaneDistance_;
        leftPlane_.point = rightPlane_.point = topPlane_.point = bottomPlane_.point = camPosition_;

        glm::mat3 modMat = glm::mat3(camRight_, camUp_, -camDirection_);
        farPlane_.normal = -camDirection_;
        nearPlane_.normal = camDirection_;
        leftPlane_.normal = modMat * leftPlane_.normal;
        rightPlane_.normal = modMat * rightPlane_.normal;
        topPlane_.normal = modMat * topPlane_.normal;
        bottomPlane_.normal = modMat * bottomPlane_.normal;

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

    /*
    * TODO: Instead of constraining, once I looked more than 90° up, switch direction again,
    * so moving down is down again
    
    if (constrainPitch_) {
        if (pitch_ > 89.0f)
            pitch_ = 89.0f;
        if (pitch_ < -89.0f)
            pitch_ = -89.0f;
    }
    */

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

bool View::isInsidePlane(Plane *plane, glm::vec3 &max, glm::vec3 &min) {
    glm::vec3 v = min;

    if (plane->normal.x >= 0)
        v.x = max.x;
    if (plane->normal.y >= 0)
        v.y = max.y;
    if (plane->normal.z >= 0)
        v.z = max.z;

    float dotP = glm::dot(v - plane->point, plane->normal);

    return dotP >= 0;
}

/* http://www.lighthouse3d.com/tutorials/view-frustum-culling/ */
bool View::isInsideFrustum(glm::vec3 &max, glm::vec3 &min) {
    for (Plane *p : frustumPlanes_) {
        if (!isInsidePlane(p, max, min))
            return false;
    }

    return true;
}