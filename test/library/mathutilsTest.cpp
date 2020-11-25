#include "gtest/gtest.h"
#include <glm/glm.hpp>
#include "mathutils.hpp"

TEST(MathUtilsTest, testBoundingBoxMinDistance) {
    BoundingBox boundingBox(glm::vec3(0,0,0), glm::vec3(1,1,1));

    float dist = boundingBox.minDistanceFromPointSq(glm::vec3(0,0,0));

    EXPECT_EQ(dist, 0.0f);
}