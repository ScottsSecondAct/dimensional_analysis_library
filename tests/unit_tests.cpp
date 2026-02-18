#include <gtest/gtest.h>
#include "dimensions.h"
#include "ecs.h"

TEST(Physics, DimensionalMultiplication) {
    auto len = 5.0_m;
    auto area = len * len;
    EXPECT_DOUBLE_EQ(area.value, 25.0);
}

TEST(ECS, ViewFiltering) {
    Registry reg;
    reg.get_pool<int>().assign(1, 10);
    reg.get_pool<float>().assign(1, 2.0f);
    reg.get_pool<int>().assign(2, 20); 

    int count = 0;
    reg.view<int, float>([&](int& i, float& f) { count++; });
    EXPECT_EQ(count, 1);
}