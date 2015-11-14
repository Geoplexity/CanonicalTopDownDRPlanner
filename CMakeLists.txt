# CMake requirements
cmake_minimum_required(VERSION 2.8)

# Project info
project(DRPlanner)

set(DRPLANNER_MAJOR_VERSION 0)
set(DRPLANNER_MINOR_VERSION 1)
set(DRPLANNER_PATCH_VERSION 0)
set(DRPLANNER_VERSION
  ${DRPLANNER_MAJOR_VERSION}.${DRPLANNER_MINOR_VERSION}.${DRPLANNER_PATCH_VERSION})




# set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/local/lib)
# set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /opt/local/include)




# Open GL
find_package(OpenGL)
include_directories(${OPENGL_INCLUDE_DIR})
# MESSAGE(STATUS "OpenGL Include dirs: " ${OPENGL_INCLUDE_DIR})
# MESSAGE(STATUS "OpenGL Libraries: " ${OPENGL_LIBRARIES})

# GLEW
find_package(GLEW)
include_directories(${GLEW_INCLUDE_DIRS})
# MESSAGE(STATUS "GLEW Include dirs: " ${GLEW_INCLUDE_DIRS})
# MESSAGE(STATUS "GLEW Libraries: " ${GLEW_LIBRARIES})

# GLFW
# find_package(GLFW REQUIRED glfw3)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRECTORIES})
link_directories(${GLFW_LIBRARY_DIRS})
# MESSAGE(STATUS "GLFW Include dirs: " ${GLFW_INCLUDE_DIRECTORIES})
# MESSAGE(STATUS "GLFW Library dirs: " ${GLFW_LIBRARY_DIRS})
# MESSAGE(STATUS "GLFW Libraries: " ${GLFW_LIBRARIES})

# Boost (Graph)
find_package(Boost 1.55 COMPONENTS graph)
include_directories(${Boost_INCLUDE_DIRS})

# External
include_directories(${PROJECT_SOURCE_DIR}/ext/include)
# link_directories(${PROJECT_SOURCE_DIR}/ext/lib)








# Set the compile time flags
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG $(CMAKE_CXX_FLAGS))





# Compile
add_subdirectory(src/Application)
add_subdirectory(src/Graph)
add_subdirectory(src/GUI)

add_executable(drp src/main.cpp)

target_link_libraries(drp Application Graph GUI)
