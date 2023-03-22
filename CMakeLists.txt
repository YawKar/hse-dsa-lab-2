cmake_minimum_required(VERSION 3.15)
project(conan_template CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(benchmark REQUIRED)

# Add subfolders
SET(IMPLEMENTATIONS src/implementations/NaiveRectangleEnumeration.cpp)
SET(COMMON src/common/TestCaseGenerator.cpp)

# Combine main with all other
SET(SRCS src/main.cpp ${IMPLEMENTATIONS} ${COMMON})

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} benchmark::benchmark)
