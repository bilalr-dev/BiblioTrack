cmake_minimum_required(VERSION 3.12)
project(BiblioTrack)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)
include_directories(config)
include_directories(src)
include_directories(src/datamodel)
include_directories(src/launcher)
include_directories(src/services)

add_executable(BiblioTrack
        src/datamodel/Book.hpp
        src/launcher/LibraryApp.cpp
        src/launcher/LibraryApp.hpp
        src/launcher/main.cpp
        src/services/AuthenticationService.cpp
        src/services/AuthenticationService.hpp
        src/services/BookRepository.cpp
        src/services/BookRepository.hpp
        src/services/LibraryService.cpp
        src/services/LibraryService.hpp
        src/launcher/main.cpp
        src/launcher/main.cpp)
