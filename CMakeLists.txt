cmake_minimum_required(VERSION 3.10)
project(Swipe VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Source files
set(SWIPE_SOURCES
    src/core.c
    src/window.c
    src/widget.c
)

# Include directories
include_directories(include)

# Shared library
add_library(swipe SHARED ${SWIPE_SOURCES})
target_include_directories(swipe PUBLIC include)

# Static library
add_library(swipe_static STATIC ${SWIPE_SOURCES})
target_include_directories(swipe_static PUBLIC include)
set_target_properties(swipe_static PROPERTIES OUTPUT_NAME swipe)

# Compiler flags
if(MSVC)
    target_compile_options(swipe PRIVATE /W4)
    target_compile_options(swipe_static PRIVATE /W4)
else()
    target_compile_options(swipe PRIVATE -Wall -Wextra -Werror -pedantic)
    target_compile_options(swipe_static PRIVATE -Wall -Wextra -Werror -pedantic)
endif()

# Example executable
add_executable(hello_window examples/hello_window.c)
target_link_libraries(hello_window swipe_static)

# Installation
install(TARGETS swipe swipe_static
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(FILES include/swipe.h DESTINATION include)
