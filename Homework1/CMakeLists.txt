cmake_minimum_required(VERSION 3.15)
project(hw1)

find_package(OpenCV 4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_compile_definitions(PROJECT_DIR="${PROJECT_SOURCE_DIR}")

#find_package(OpenCV 4 REQUIRED)

set(CMAKE_CXX_STANDARD 14)

add_executable(hw homework1.cpp)
#target_link_libraries(demo2 ${OpenCV_LIBS} Eigen3::Eigen)
target_link_libraries(hw ${OpenCV_LIBS})
