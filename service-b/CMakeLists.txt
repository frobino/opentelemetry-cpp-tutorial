cmake_minimum_required(VERSION 3.6)

# Create a few variables, used to make this file modular
set(PROJECT_NAME serviceB)

# Name of the project (which will be the 1st default target)
project(${PROJECT_NAME} VERSION 1.0)

# Pistache requires C++ 17
set(CMAKE_CXX_STANDARD 17)

# conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Executable
add_executable(${PROJECT_NAME} main.cpp)

add_library(opentelemetry_exporter_jaeger_trace STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(opentelemetry_exporter_jaeger_trace PROPERTIES
  IMPORTED_LOCATION "/home/efroroo/.conan/data/opentelemetry-cpp/1.0.1/_/_/package/864fac862bdafe48e6890da6667fded3c4d61821/lib/libopentelemetry_exporter_jaeger_trace.a"
  INTERFACE_INCLUDE_DIRECTORIES "/home/efroroo/.conan/data/opentelemetry-cpp/1.0.1/_/_/package/864fac862bdafe48e6890da6667fded3c4d61821/include/"
)

add_library(opentelemetry_resources STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(opentelemetry_resources PROPERTIES
  IMPORTED_LOCATION "/home/efroroo/.conan/data/opentelemetry-cpp/1.0.1/_/_/package/864fac862bdafe48e6890da6667fded3c4d61821/lib/libopentelemetry_resources.a"
  INTERFACE_INCLUDE_DIRECTORIES "/home/efroroo/.conan/data/opentelemetry-cpp/1.0.1/_/_/package/864fac862bdafe48e6890da6667fded3c4d61821/include/"
)

add_library(opentelemetry_trace STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(opentelemetry_trace PROPERTIES
  IMPORTED_LOCATION "/home/efroroo/.conan/data/opentelemetry-cpp/1.0.1/_/_/package/864fac862bdafe48e6890da6667fded3c4d61821/lib/libopentelemetry_trace.a"
  INTERFACE_INCLUDE_DIRECTORIES "/home/efroroo/.conan/data/opentelemetry-cpp/1.0.1/_/_/package/864fac862bdafe48e6890da6667fded3c4d61821/include/"
)

add_library(opentelemetry_common STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(opentelemetry_common PROPERTIES
  IMPORTED_LOCATION "/home/efroroo/.conan/data/opentelemetry-cpp/1.0.1/_/_/package/864fac862bdafe48e6890da6667fded3c4d61821/lib/libopentelemetry_common.a"
  INTERFACE_INCLUDE_DIRECTORIES "/home/efroroo/.conan/data/opentelemetry-cpp/1.0.1/_/_/package/864fac862bdafe48e6890da6667fded3c4d61821/include/"
)

add_library(http_client_curl STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(http_client_curl PROPERTIES
  IMPORTED_LOCATION "/home/efroroo/.conan/data/opentelemetry-cpp/1.0.1/_/_/package/864fac862bdafe48e6890da6667fded3c4d61821/lib/libhttp_client_curl.a"
  INTERFACE_INCLUDE_DIRECTORIES "/home/efroroo/.conan/data/opentelemetry-cpp/1.0.1/_/_/package/864fac862bdafe48e6890da6667fded3c4d61821/include/"
)

# Subdirectories
# add_subdirectory("../common/opentelemetry-cpp" "${CMAKE_CURRENT_BINARY_DIR}/opentelemetry-cpp_build")
# add_subdirectory("../common/pistache" "${CMAKE_CURRENT_BINARY_DIR}/pistache_build")

# Dependencies
# include_directories("../common/opentelemetry-cpp/exporters/jaeger/include")
# target_link_libraries(${PROJECT_NAME} PUBLIC 
#     opentelemetry_trace 
#     jaeger_trace_exporter
#     pistache
#     thrift::thrift_static)

# Dependencies:
# Link the executable and the library together
target_link_libraries(${PROJECT_NAME} opentelemetry_exporter_jaeger_trace opentelemetry_resources opentelemetry_trace opentelemetry_common http_client_curl ${CONAN_LIBS})
