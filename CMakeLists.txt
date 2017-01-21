cmake_minimum_required(VERSION 2.7)

# Name of the project
project(GraphicsBoilerplate)

# Use glob to get the list of all source files.
file(GLOB_RECURSE SOURCES "src/*.cpp")

# We don't really need to include header and resource files to build, but it's
# nice to have them show up in IDEs.
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE GLSL "resources/*.glsl")

# Set the executable.
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${GLSL})

# Since GLM is a header-only library,
# we just need to add it to the include directory.
# TODO check if the git submodule was populated first?
include_directories("deps/glm")

# There should be a CMakeLists.txt in the specified directory.
set(GLFW_DIR "deps/glfw")
option(GLFW_BUILD_EXAMPLES "GLFW_BUILD_EXAMPLES" OFF)
option(GLFW_BUILD_TESTS "GLFW_BUILD_TESTS" OFF)
option(GLFW_BUILD_DOCS "GLFW_BUILD_DOCS" OFF)
if(CMAKE_BUILD_TYPE MATCHES Release)
  add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/release)
else()
  add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/debug)
endif()
include_directories(${GLFW_DIR}/include)
target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})

set(GLEW_DIR "deps/glew")
#set(GLEW_DIR "/home/jarhar/glew-tar")
include_directories(${GLEW_DIR}/include)
if(WIN32)
  # With prebuilt binaries
  target_link_libraries(${CMAKE_PROJECT_NAME} ${GLEW_DIR}/lib/Release/Win32/glew32s.lib)
else()
  target_link_libraries(${CMAKE_PROJECT_NAME} ${GLEW_DIR}/lib/libGLEW.a)
endif()

# OS specific options and libraries
if(WIN32)
  # c++0x is enabled by default.
  # -Wall produces way too many warnings.
  # -pedantic is not supported.
else()
  # Enable all pedantic warnings.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -w -g")
  if(APPLE)
    # Add required frameworks for GLFW.
    target_link_libraries(${CMAKE_PROJECT_NAME} "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
  else()
    #Link the Linux OpenGL library
    target_link_libraries(${CMAKE_PROJECT_NAME} "GL")
  endif()
endif()
