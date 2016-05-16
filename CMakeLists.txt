cmake_minimum_required (VERSION 2.8.11)
project(Redis_cpp_client)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no
    C++11 support. Please use a different C++ compiler.")
endif()

# -------------------------------------------
# Include directories
# -------------------------------------------
include_directories(include)


# -------------------------------------------
# Source files
# -------------------------------------------
file(GLOB SOURCES "src/*.cpp")
file(GLOB EXAMPLE_SOURCES "examples/*.cpp")

# -------------------------------------------
# Dependencies
# -------------------------------------------
set(DEPENDENCIES hiredis jsoncpp)

add_library(rediscpp SHARED ${SOURCES})
add_executable(example ${EXAMPLE_SOURCES})

target_link_libraries(rediscpp ${DEPENDENCIES})
target_link_libraries(example rediscpp)
