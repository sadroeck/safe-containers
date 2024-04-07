cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    safe-containers
    VERSION 0.1.0
    DESCRIPTION "C++17/20 STL containers with support for fallible allocation"
    HOMEPAGE_URL "https://github.com/sadroeck/safe-containers"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(safe-containers_safe-containers INTERFACE)
add_library(safe-containers::safe-containers ALIAS safe-containers_safe-containers)

set_property(
    TARGET safe-containers_safe-containers PROPERTY
    EXPORT_NAME safe-containers
)

target_include_directories(
    safe-containers_safe-containers ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(safe-containers_safe-containers INTERFACE cxx_std_17)

find_package(fmt REQUIRED)
target_link_libraries(safe-containers_safe-containers INTERFACE fmt::fmt)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT safe-containers_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of safe-containers"
  )
endif()

include(cmake/dev-mode.cmake)
