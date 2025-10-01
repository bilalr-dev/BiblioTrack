cmake_minimum_required(VERSION 3.12)
project(LibraryManagementSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(library_app ${SOURCES})

target_compile_options(library_app PRIVATE -Wall -Wextra -Wpedantic)

# Define data directory for runtime
target_compile_definitions(library_app PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}/data")

# Create data directory at build time
add_custom_command(TARGET library_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_SOURCE_DIR}/data
)

