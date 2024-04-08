cmake_minimum_required(VERSION 3.14)

project(safe-containersTests LANGUAGES CXX)

include(../cmake/folders.cmake)

# ---- Dependencies ----

if (PROJECT_IS_TOP_LEVEL)
    find_package(safe-containers REQUIRED)
    enable_testing()
endif ()

find_package(GTest CONFIG REQUIRED)

# ---- Tests ----

add_executable(safe-containers_test
        source/test_vector.cpp
        source/test_result_ext.cpp)
TARGET_INCLUDE_DIRECTORIES(safe-containers_test PRIVATE ${GTest_INCLUDE_DIRS})
target_link_libraries(
        safe-containers_test PRIVATE
        safe-containers::safe-containers
        GTest::gtest
        GTest::gtest_main
        GTest::gmock GTest::gmock_main
)
target_compile_features(safe-containers_test PRIVATE cxx_std_17)

# ---- End-of-file commands ----

add_folders(Test)
