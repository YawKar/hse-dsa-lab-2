# conan-template-setup
Template c++ project with conan setup

# How to setup and run
1. Install conan 2.0
2. [Optional] Create conan profile if you haven't yet: `conan profile detect --force`. This will create new default profile for conan in your system.
3. Install CMake of version not less than the one that is specified in CMakeLists.txt
3. `cd` to the root folder of the project
4. `conan install . --output-folder=build --build=missing`
5. `cd build`
6. `cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release`
7. `cmake --build .`
8. `./conan_template`

Now make a tea and eat some cookies. You're awesome.
