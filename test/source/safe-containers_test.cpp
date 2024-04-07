#include "safe-containers/safe-containers.hpp"

#include <gtest/gtest.h>

TEST(safe_vector, hello_world)
{
    EXPECT_EQ(name(), "safe-containers");
}
