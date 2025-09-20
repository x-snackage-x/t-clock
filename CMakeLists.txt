cmake_minimum_required(VERSION 3.16)
project(t-clock VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set the output directory for executables to the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Generate a Makefile
if (MSVC)
    set(CMAKE_GENERATOR "NMake Makefiles")
else()
    set(CMAKE_GENERATOR "Unix Makefiles")
endif()

add_executable(t-clock t-clock.c)

