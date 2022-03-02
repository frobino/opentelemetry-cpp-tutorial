cmake_minimum_required(VERSION 3.6)

# Hunter is used to get thrift (a package required by jaeger-exporter)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(HunterGate)

option(HUNTER_BUILD_SHARED_LIBS "Build Shared Library" ON)

HunterGate(
   URL "https://github.com/cpp-pm/hunter/archive/v0.23.249.tar.gz"
   SHA1 "d45d77d8bba9da13e9290a180e0477e90accd89b"
   LOCAL # load `${CMAKE_CURRENT_LIST_DIR}/cmake/Hunter/config.cmake`
)

# Project name and version
project(serviceB VERSION 1.0)

# Get thrift
hunter_add_package(thrift)
find_package(thrift CONFIG REQUIRED)

# Pistache requires C++ 17
set(CMAKE_CXX_STANDARD 17)

# Executable
add_executable(${PROJECT_NAME} main.cpp)

# Subdirectories
add_subdirectory("../common/opentelemetry-cpp" "${CMAKE_CURRENT_BINARY_DIR}/opentelemetry-cpp_build")
add_subdirectory("../common/pistache" "${CMAKE_CURRENT_BINARY_DIR}/pistache_build")

# Dependencies
include_directories("../common/opentelemetry-cpp/exporters/jaeger/include")
target_link_libraries(${PROJECT_NAME} PUBLIC 
    opentelemetry_trace 
    jaeger_trace_exporter        
    pistache
    thrift::thrift_static)