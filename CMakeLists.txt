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
        src/services/LibraryService.hpp)

# Test executables
add_executable(UnitTests
        tests/UnitTests.cpp
        src/services/LibraryService.cpp
        src/services/BookRepository.cpp
        src/services/AuthenticationService.cpp)

add_executable(IntegrationTests
        tests/IntegrationTests.cpp
        src/services/LibraryService.cpp
        src/services/BookRepository.cpp
        src/services/AuthenticationService.cpp)

add_executable(SmokeTests
        tests/SmokeTests.cpp
        src/services/LibraryService.cpp
        src/services/BookRepository.cpp
        src/services/AuthenticationService.cpp)

add_executable(TestRunner
        tests/TestRunner.cpp)

# Test target to run all tests
add_custom_target(run_all_tests
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/TestRunner --all
        DEPENDS UnitTests IntegrationTests SmokeTests TestRunner
        COMMENT "Running all test suites")
