#include "gtest/gtest.h"
#include <glm/glm.hpp>
#include "drawable.hpp"

TEST(DrawableTest, testInit) {
    Drawable drawable;

    EXPECT_EQ(drawable.getPosition(), glm::vec3(0,0,0));
}