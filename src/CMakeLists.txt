
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

#==============================================
#                 Build Options
#==============================================

#==============================================
# --------------------------------------------
#==============================================

# include_directories(Engine)

# if(MSVC)
#   link_directories(../lib)
#   include_directories(../include/freetype)
#   SET(RAPTOR_LIBS
#     opengl32.lib
#     assimpd.lib
#     FreeImage.lib
#     freetype.lib
#     SDL2.lib
#     SDL2main.lib
#     SDL2test.lib
#     glew32.lib
#     glfw3.lib
#   )
# endif(MSVC)
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
add_definitions(-std=c++14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../CMake/Modules/")

#User end configuration
set(BUILD_TARGET "BuildTarget" CACHE STRING "Build target chosen by user at configure time")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS HTML5 Windows Linux Mac Android IOS XONE X360 PS3 PS4)

add_definitions(-D_SDL)

if(EMSCRIPTEN)
set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif(EMSCRIPTEN)

#EMSCRIPTEN allready handles these and MSVC does its own thing...
if(NOT EMSCRIPTEN AND NOT MSVC)
  foreach(lib OpenGL GLEW SDL SDLIMAGE)
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

#That was the unix stuff
#Here is what MSVC will do
if(MSVC)
  include_directories(../include)
  link_directories(../lib)
  set(LBIS
glew32.lib
glew32s.lib
opengl32.lib
freetype.lib
SDL2.lib
SDL2main.lib
SDL2_image.lib
glfw3.lib
    )
endif(MSVC)


include_directories(${INCS})
#Debug ouput the includes and libs that were found
message("Included directories: " ${INCS})
message("Using libaries: " ${LIBS})

#Setup the include directories
include_directories(Engine)

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
      Engine/Graphics/Pixmap.hpp
      Engine/Graphics/Font.hpp
      Engine/Graphics/Font.cpp
      Engine/Graphics/Mesh.cpp
      Engine/Graphics/Mesh.hpp
      Engine/Graphics/Model.cpp
      Engine/Graphics/Model.hpp
      Engine/Graphics/Color.hpp
      Engine/Graphics/DebugRenderer.cpp
      Engine/Graphics/DebugRenderer.hpp
      Engine/Graphics/GLSLProgram.cpp
      Engine/Graphics/GLSLProgram.hpp
)
add_library(graphics STATIC ${GRAPHICS_SRCS})

set(MATH_SRCS
      Engine/Math/Matrix4.cpp
      Engine/Math/Matrix4.hpp
      Engine/Math/Vector2.hpp
      Engine/Math/Vector3.hpp
      Engine/Math/Vector3.cpp
)
add_library(math STATIC ${MATH_SRCS})

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

  # Engine/Base/PhysicsSystem2D.cpp
  # Engine/Base/PhysicsSystem3D.hpp
)
add_library(base STATIC ${BASE_SRCS})

set(ECS_SRCS
  Engine/ECS/Component.hpp
  Engine/ECS/ComponentBlock.cpp
  Engine/ECS/ComponentType.hpp
  Engine/ECS/ECSManager.cpp
  Engine/ECS/ECSManager.hpp
  Engine/ECS/Entity.hpp
  Engine/ECS/ISystem.hpp
)
add_library(ecs STATIC ${ECS_SRCS})

#base depends on ecs
target_link_libraries(base ecs graphics)

SET(PLATFORM_SRCS
  Engine/Utils/IO.cpp
  Engine/Utils/IO.hpp
  Engine/Platform/Platform.hpp
  Engine/Platform/SDLPlatform.cpp
)

add_library(run STATIC ${PLATFORM_SRCS})

add_executable(app Tests/Main.cpp)
target_link_libraries(app run)
target_link_libraries(app ecs base)
target_link_libraries(app graphics)
target_link_libraries(app math)
target_link_libraries(app ${LIBS})
# target_link_libraries(app math)
# target_link_libraries(app base)
# target_link_libraries(app graphics)

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
# source_group(Tools FILES ${TOOLS_SRCS})
