cmake_minimum_required(VERSION 3.15)
project(conan_template CXX)

find_package(fmt REQUIRED)

SET(SRCS src/main.cpp)

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} fmt::fmt)

