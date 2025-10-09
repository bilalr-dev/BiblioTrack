cmake_minimum_required(VERSION 3.12)
project(BiblioTrack VERSION 2.06 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# No external dependencies for authentication (uses JSON file)

# Source files
set(SRC
        src/services/BookRepository.cpp
        src/services/LibraryService.cpp
        src/services/AuthenticationService.cpp
        src/launcher/LibraryApp.cpp
        src/launcher/main.cpp
)

# Headers are added for IDE visibility (not required to compile)
set(HEADERS
        src/datamodel/Book.hpp
        src/services/BookRepository.hpp
        src/services/LibraryService.hpp
        src/services/AuthenticationService.hpp
        src/launcher/LibraryApp.hpp
        src/launcher/main.cpp
)

add_executable(bibliotrack ${SRC} ${HEADERS})

target_include_directories(bibliotrack PRIVATE src)

target_compile_options(bibliotrack PRIVATE -Wall -Wextra)

# No external libraries needed

# Ensure data directory exists for runtime
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)
