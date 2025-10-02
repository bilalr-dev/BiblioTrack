cmake_minimum_required(VERSION 3.12)
project(BiblioTrack VERSION 2.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files organized by layers
set(DATAMODEL_SOURCES
    src/datamodel/Book.hpp
)

set(SERVICES_SOURCES
    src/services/BookRepository.cpp
    src/services/LibraryService.cpp
)

set(LAUNCHER_SOURCES
    src/launcher/LibraryApp.cpp
    src/launcher/main.cpp
)

# All source files
set(SOURCES
    ${DATAMODEL_SOURCES}
    ${SERVICES_SOURCES}
    ${LAUNCHER_SOURCES}
)

# Create executable
add_executable(bibliotrack ${SOURCES})

# Include directories for clean imports
target_include_directories(bibliotrack PRIVATE src)

# Compiler flags
target_compile_options(bibliotrack PRIVATE -Wall -Wextra)

# Create data directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)