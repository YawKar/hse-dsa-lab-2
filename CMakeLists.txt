cmake_minimum_required(VERSION 3.15)
project(lab_benchmarks CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(benchmark REQUIRED)

# Add subfolders
SET(IMPLEMENTATIONS 
    src/implementations/NaiveRectangleEnumeration.cpp 
    src/implementations/QubicMapBuilding.cpp
    src/implementations/PersistentSegmentTree.cpp)
SET(COMMON src/common/TestCaseGenerator.cpp)

# Combine main with all other
SET(SRCS src/main.cpp ${IMPLEMENTATIONS} ${COMMON})

add_executable(${PROJECT_NAME} ${SRCS})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion)
endif()

target_link_libraries(${PROJECT_NAME} benchmark::benchmark)
