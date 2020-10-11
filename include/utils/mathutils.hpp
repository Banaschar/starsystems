#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

struct Plane {
    glm::vec3 point;
    glm::vec3 normal;
};

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;

    BoundingBox() {}
    BoundingBox(glm::vec3 minPos, glm::vec3 maxPos) : min(minPos), max(maxPos) {}

    float minDistanceFromPointSq(glm::vec3 point) {
        float dist = 0.0f;

        if (point.x < min.x) {
            float d = point.x - min.x;
            dist += d*d;
        } else if (point.x > max.x) {
            float d = point.x - max.x;
            dist += d*d;
        }

        if (point.y < min.y) {
            float d = point.y - min.y;
            dist += d*d;
        } else if (point.y > max.y) {
            float d = point.y - max.y;
            dist += d*d;
        }

        if (point.z < min.z) {
            float d = point.z - min.z;
            dist += d*d;
        } else if (point.z > max.z) {
            float d = point.z - max.z;
            dist += d*d;
        }
        return dist;
    }

    bool intersectSphereSq(const glm::vec3 &point, float radiusSq) {
        return minDistanceFromPointSq(point) <= radiusSq;
    }
};
#endif