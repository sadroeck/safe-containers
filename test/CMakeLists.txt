cmake_minimum_required(VERSION 3.14)

project(safe-containersTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(safe-containers REQUIRED)
  enable_testing()
endif()

find_package(gtest REQUIRED)
include(gtest)

# ---- Tests ----

add_executable(safe-containers_test source/safe-containers_test.cpp)
target_link_libraries(
    safe-containers_test PRIVATE
    safe-containers::safe-containers
    Catch2::Catch2WithMain
)
target_compile_features(safe-containers_test PRIVATE cxx_std_17)

catch_discover_tests(safe-containers_test)

# ---- End-of-file commands ----

add_folders(Test)
