cmake_minimum_required(VERSION 3.3)
set(BUILD_TARGET "BuildTarget" CACHE STRING "Build target chosen by user at configure time")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS HTML Windows Linux Mac Android IOS)

project(Raptor)
if (NOT MSVS)
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif(NOT MSVS)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules/")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

if(NOT EMSCRIPTEN)
    add_definitions(-DPLATFORM_SDL)
else()
	add_definitions(-DPLATFORM_HTML5)
endif(NOT EMSCRIPTEN)

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif(EMSCRIPTEN)

if(NOT EMSCRIPTEN AND NOT MSVC)
  foreach(lib OpenGL GLEW SDL2 SDL2_mixer SDL2_net ASSIMP)
    find_package(${lib} REQUIRED)
    string(TOUPPER ${lib} lib)
    if(NOT ${lib}_FOUND)
      message("Could not find" ${lib}! FATAL_ERROR)
    endif(NOT ${lib}_FOUND)
    list(APPEND INCS ${${lib}_INCLUDE_DIRS})
    list(APPEND LIBS ${${lib}_LIBRARIES})
    list(APPEND LIBS ${${lib}_LIBRARY})
  endforeach(lib GLEW SDL)
endif()

if(MSVC)
  include_directories(include)
  include_directories(C:/usr/include)
  link_directories(C:/usr/lib/Release)
  link_directories(C:/usr/lib/Debug)
  link_directories(lib)
  set(LIBS
    opengl32.lib
    glew32s.lib
    SDL2.lib
    SDL2_image.lib
    SDL2_mixer.lib
    SDL2_net.lib
    assimp.lib


  )

set(RELEASE_LIBS
    LinearMath
    BulletCollision
    BulletDynamics
    BulletSoftBody
)

set(DEBUG_LIBS
    LinearMath_Debug
    BulletCollision_Debug
    BulletDynamics_Debug
    BulletSoftBody_Debug
)

endif(MSVC)


link_directories(build/src/Box2D)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(tools)
