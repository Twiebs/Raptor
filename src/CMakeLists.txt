
# # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
# # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
# if(NOT MSVC AND NOT EMCC)
# #  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -I/usr/include/freetype2)
# endif(NOT MSVC)
#
# #Set the paths Cmake will search for other .cmake files
# set(CMAKE_MODULE_PATH
#     ${CMAKE_SOURCE_DIR}/../CMake
#     ${CMAKE_SOURCE_DIR}/../CMake/Packages
# )
#
# include(Dependencies)
#
#
# # Avoid source tree pollution
# if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
#   message(FATAL_ERROR "Build directory must not be the same as the source dir")
# endif(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
#
#

#
# #Build the RaptorEngine as a static lib
# #The below directories are used to create a list of files to pass to the add_library function
# set(ENGINE_SRC_DIRS Application Assets Components Core Graphics Graphics/GL Math Memory Platform Systems UI Utils)
# foreach(DIR ${ENGINE_SRC_DIRS})
#   file(GLOB ENGINE_${DIR}_CPP Engine/${DIR}/*.cpp)
#   file(GLOB ENGINE_${DIR}_HPP Engine/${DIR}/*.hpp)
#   set(ENGINE_${DIR}_SRCS ${ENGINE_${DIR}_CPP} ${ENGINE_${DIR}_HPP})
#   list(APPEND ENGINE_ALL_SRCS ${ENGINE_${DIR}_SRCS})
# endforeach(DIR ${ENGINE_SRC_DIRS})
#
# #Add the sources that were appended to the library
# # add_library(RaptorCore STATIC ${ENGINE_ALL_SRCS})
# #
# # #Generate MSVS Filters for our files
# # foreach(DIR ${ENGINE_SRC_DIRS})
# #   source_group(${DIR} FILES ${ENGINE_${DIR}_SRCS})
# # endforeach(DIR ${ENGINE_SRC_DIRS})


cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Raptor)
if(NOT MSVC)
  add_definitions(-std=c++14)
endif(NOT MSVC)

add_subdirectory(imgui)
add_subdirectory(Box2D)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../CMake/Modules/")

#User end configuration
set(BUILD_TARGET "BuildTarget" CACHE STRING "Build target chosen by user at configure time")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS HTML5 Windows Linux Mac Android IOS XONE X360 PS3 PS4)

if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
else(EMSCRPITEN)
  add_definitions(-DSDL)
endif(EMSCRIPTEN)

#EMSCRIPTEN allready handles these and MSVC does its own thing...
if(NOT EMSCRIPTEN AND NOT MSVC)
  foreach(lib OpenGL GLEW SDL2 SDL2_image SDL2_mixer SDL2_net)
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

#Now we add Dependencies that EMSCRIPTEN will not cover
if(NOT EMSCRIPTEN AND NOT MSVC)
  foreach(lib ASSIMP FreeImage Freetype )
    find_package(${lib} REQUIRED)
    string(TOUPPER ${lib} lib)
    if(NOT ${lib}_FOUND)
      message("Could not find" ${lib}! FATAL_ERROR)
    endif(NOT ${lib}_FOUND)
    list(APPEND INCS ${${lib}_INCLUDE_DIRS})
    list(APPEND LIBS ${${lib}_LIBRARIES})
    list(APPEND LIBS ${${lib}_LIBRARY})
  endforeach(lib FreeImage)
endif()


if(MSVC)
  include_directories(../include)
  link_directories(../lib)
  set(LIBS
    glew32.lib
    glew32s.lib
    opengl32.lib
    freetype.lib
    SDL2.lib
    SDL2_image.lib
    SDL2_mixer.lib
    SDL2_net.lib
  )
endif(MSVC)


include_directories(${INCS})
#Debug ouput the includes and libs that were found
message("Included directories: " ${INCS})
message("Using libaries: " ${LIBS})

#Setup the include directoriess
include_directories(Engine)
include_directories(../src)
include_directories(imgui)

#This is to deal with freetypes sillyness...
#not only is it silly but windows and linux want it two different ways!
if(MSVC)
	include_directories(../include/freetype)
else()
	include_directories(/usr/include/freetype2)
endif()

# set(CORE_SRCS
#   Engine/Core/AssetManager.cpp
#   Engine/Core/AssetManager.hpp
#   Engine/Core/TaskManager.cpp
#   Engine/Core/TaskManager.hpp
#   Engine/Assets/LoadShaderTask.cpp
#   Engine/Assets/LoadShaderTask.hpp
#   Engine/Assets/ModelLoader.cpp
#   Engine/Assets/ModelLoader.hpp
#   Engine/Assets/IAssetLoader.cpp
#   Engine/Assets/IAssetLoader.hpp
# )

# add_library(core STATIC ${CORE_SRCS})
# target_link_libraries(core ${LIBS} math graphics)

set(GRAPHICS_SRCS
      Engine/Graphics/Texture.cpp
      Engine/Graphics/Texture.hpp
      Engine/Graphics/Pixmap.hpp
      Engine/Graphics/Font.hpp
      Engine/Graphics/Font.cpp
      Engine/Graphics/Model.cpp
      Engine/Graphics/Model.hpp
      Engine/Graphics/Color.hpp
      Engine/Graphics/DEBUGRenderer.cpp
      Engine/Graphics/DEBUGRenderer.hpp
      Engine/Graphics/GLSLProgram.cpp
      Engine/Graphics/GLSLProgram.hpp
)
add_library(Graphics STATIC ${GRAPHICS_SRCS})
source_group(Graphics FILES ${GRAPHICS_SRCS})

set(MATH_SRCS
  Engine/Math/Noise.cpp
  Engine/Math/Noise.hpp
  Engine/Math/Random.cpp
  Engine/Math/Random.hpp
  Engine/Math/Matrix4.cpp
  Engine/Math/Matrix4.hpp
  Engine/Math/Vector2.cpp
  Engine/Math/Vector2.hpp
  Engine/Math/Vector3.hpp
  Engine/Math/Vector3.cpp
)
add_library(Math STATIC ${MATH_SRCS})
source_group(Math FILES ${MATH_SRCS})

set(BASE_SRCS
  # Engine/Base/MeshComponent.hpp
  # Engine/Base/ModelComponent.hpp
  Engine/Base/SpriteComponent.hpp
  Engine/Base/TextComponent.hpp
  #
  Engine/Base/Transform2D.hpp
  # Engine/Base/Transform3D.hpp

  Engine/Base/RenderSystem2D.cpp
  Engine/Base/RenderSystem2D.hpp
  # Engine/Base/RenderSystem3D.cpp
  # Engine/Base/RenderSystem3D.hpp

  Engine/Base/PhysicsSystem2D.cpp
  Engine/Base/PhysicsSystem2D.hpp
  # Engine/Base/PhysicsSystem3D.hpp
)
add_library(Base STATIC ${BASE_SRCS})
source_group(Base FILES ${BASE_SRCS})
target_link_libraries(Base Box2D ECS Graphics)

set(ECS_SRCS
  Engine/ECS/Component.hpp
  Engine/ECS/ComponentBlock.cpp
  Engine/ECS/ComponentType.hpp
  Engine/ECS/ECSManager.cpp
  Engine/ECS/ECSManager.hpp
  Engine/ECS/ISystem.hpp
)
add_library(ECS STATIC ${ECS_SRCS})
source_group(ECS FILES ${ECS_SRCS})

SET(PLATFORM_SRCS
  Engine/Core/Network.hpp
  Engine/Core/Network.cpp
  Engine/Platform/ImGui.hpp
  Engine/Application/SDLApplication.cpp
  Engine/Application/EmscriptenApplication.cpp
  Engine/Application/Application.hpp
  Engine/Utils/IO.cpp
  Engine/Utils/IO.hpp
)
add_library(Platform STATIC ${PLATFORM_SRCS})
source_group(Platform FILES ${PLATFORM_SRCS})

set(EMSCRIPTEN_ASSET_PACK_FLAG "--preload-file ../Assets")

add_executable(app Tests/Main.cpp)
#Ensure EMCSCRIPTEN is given enough memory to work with...
if(EMSCRIPTEN)
  target_link_libraries(app "-s TOTAL_MEMORY=134217728")
  target_link_libraries(app ${EMSCRIPTEN_ASSET_PACK_FLAG})
endif()

target_link_libraries(app imgui)
target_link_libraries(app Graphics)
target_link_libraries(app Box2D)
target_link_libraries(app Platform)
target_link_libraries(app ECS Base)
target_link_libraries(app Math)
target_link_libraries(app ${LIBS})

# file(GLOB HTML_TESTS_SRCS_CPP HTMLTests/*.cpp)
# file(GLOB HTML_TESTS_SRCS_HPP HTMLTests/*.hpp)
# set(HTML_TESTS_SRCS_ALL ${HTML_TESTS_SRCS_CPP} ${HTML_TESTS_SRCS_HPP})
# add_executable(HTMLTests.html ${HTML_TESTS_SRCS_ALL})
# target_link_libraries(HTMLTests.html RaptorRuntime)

# file(GLOB TOOLS_SRCS Tools/*.cpp)
# add_executable(RaptorTools ${TOOLS_SRCS})
# target_link_libraries(RaptorTools RaptorCore)
# set(CMAKE_LIBRARY_PATH ../lib/${CMAKE_BUILD_TYPE})
# target_link_libraries(RaptorTools ${RAPTOR_LIBS})
#
