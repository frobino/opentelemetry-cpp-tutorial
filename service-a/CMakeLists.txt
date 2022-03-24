cmake_minimum_required(VERSION 3.6)

# Create a few variables, used to make this file modular
set(PROJECT_NAME serviceA)

# Name of the project (which will be the 1st default target)
project(${PROJECT_NAME} VERSION 1.0)

# Pistache requires C++ 17
set(CMAKE_CXX_STANDARD 17)

# conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Executable
add_executable(${PROJECT_NAME} main.cpp)

# Dependencies:
# Link the executable and the library together
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
