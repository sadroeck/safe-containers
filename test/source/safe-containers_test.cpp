#include "safe-containers/safe-containers.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Name is safe-containers", "[library]")
{
  REQUIRE(name() == "safe-containers");
}
