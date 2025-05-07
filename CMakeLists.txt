cmake_minimum_required(VERSION 3.10)
project(ttfRenderer)

# Set C flags
set(CMAKE_C_FLAGS "-Wall -g -Wno-implicit-function-declaration")

# Export compile commands for language server
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add include directorie
include_directories(include)

# Find packages
find_package(SDL2 REQUIRED)
find_package(OpenGL QUIET)

# Define the source files
file(GLOB_RECURSE SOURCES "src/*.c")

# Create the executable
add_executable(main ${SOURCES})

# Link libraries
target_link_libraries(main SDL2::SDL2)
if(OPENGL_FOUND)
  target_link_libraries(main ${OPENGL_LIBRARIES})
  target_include_directories(main PRIVATE ${OPENGL_INCLUDE_DIR})
else()
  message("OpenGL could not be found, trying direct linking as fallback.")
  target_link_libraries(main GL)
endif()
