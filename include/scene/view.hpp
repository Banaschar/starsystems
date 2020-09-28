#ifndef VIEW_H
#define VIEW_H

#include <functional>
#include <glm/glm.hpp>

struct GLFWwindow;

struct Plane {
    glm::vec3 point;
    glm::vec3 normal;
};

class View {
  public:
    View(GLFWwindow *window, glm::vec3 camPos);
    View(GLFWwindow *window, glm::vec3 camPos, glm::vec3 camDir);
    View(GLFWwindow *window, glm::vec3 camPos, glm::vec3 camDir, float angleH, float angleVert, float fov, float speed,
         float mouseSpeed);
    glm::mat4 &getCameraMatrix();
    glm::mat4 &getProjectionMatrix();
    glm::vec3 &getCameraPosition();
    /*
     * The direction the camera is looking at.
     * Already normalized
     */
    glm::vec3 &getCameraDirection();
    glm::mat4 getOrthoProjection();
    glm::vec3 &getWorldNormal();
    float getNearPlane();
    float getFarPlane();
    void invertPitch();
    void update();
    void updateForce();
    void setAutoRotate(bool value);
    void mouseCallback(GLFWwindow *window, double xpos, double ypos);
    void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    void setupInput();
    void getWindowSize(int *width, int *height);
    // std::function<void(GLFWwindow*, double, double)> mCb;
    // std::function<void(GLFWwindow*, double, double)> sCb;
    void (*mCb)(GLFWwindow *, double, double);
    void (*sCb)(GLFWwindow *, double, double);
    bool isInsideFrustum(glm::vec3 &max, glm::vec3 &min);

  private:
    void rotateCamera();
    void processInputs();
    void update_();
    GLFWwindow *window_;
    int windowWidth_, windowHeight_;
    glm::mat4 cameraMatrix_;
    glm::mat4 projectionMatrix_;
    glm::vec3 camPosition_;
    glm::vec3 camDirection_;
    glm::vec3 camUp_;
    glm::vec3 camPositionOriginal_;
    glm::vec3 worldUp_;
    glm::vec3 camRight_;
    bool autoRotate_;
    bool firstMouseMove_ = true;
    bool flagUpdate_ = false;
    bool constrainPitch_;
    double lastX_;
    double lastY_;
    float fov_;
    float speed_;
    float cameraSpeed_;
    float camRotateVal_;
    float zoom_;
    float mouseSensitivity_;
    float yaw_;
    float pitch_;
    float nearPlaneDistance_;
    float farPlaneDistance_;
    Plane nearPlane_;
    Plane farPlane_;
    Plane leftPlane_;
    Plane rightPlane_;
    Plane topPlane_;
    Plane bottomPlane_;
    std::vector<Plane *> frustumPlanes_;

    bool isInsidePlane(Plane *plane, glm::vec3 &max, glm::vec3 &min);
};
#endif