
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../CMake/Modules/")

#User end configuration
set(BUILD_TARGET "BuildTarget" CACHE STRING "Build target chosen by user at configure time")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS HTML5 Windows Linux Mac Android IOS XONE X360 PS3 PS4)

add_subdirectory(imgui)

if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
else(EMSCRPITEN)
  add_definitions(-DSDL)
endif(EMSCRIPTEN)

#EMSCRIPTEN allready handles these and MSVC does its own thing...
if(NOT EMSCRIPTEN AND NOT MSVC)
  foreach(lib OpenGL GLEW SDL2 SDL2_mixer SDL2_net)
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

set(GRAPHICS_SRCS
	  Engine/Graphics/GUI.cpp
	  Engine/Graphics/GUI.hpp
      Engine/Graphics/Texture.cpp
      Engine/Graphics/Texture.hpp
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
  Engine/Base/Transform2D.hpp
  # Engine/Base/Transform3D.hpp

  # Engine/Base/RenderSystem2D.cpp
  # Engine/Base/RenderSystem2D.hpp
  # Engine/Base/RenderSystem3D.cpp
  # Engine/Base/RenderSystem3D.hpp

  # Engine/Base/PhysicsSystem2D.cpp
  # Engine/Base/PhysicsSystem2D.hpp
  # Engine/Base/PhysicsSystem3D.hpp
)
#add_library(Base STATIC ${BASE_SRCS})
source_group(Base FILES ${BASE_SRCS})

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

set(TEST_SRCS
	Tests/Main.cpp
	Test/NetworkTest.cpp
)
source_group(Tests FILES ${TEST_SRCS})

SET(CORE_SRCS
  Engine/Core/Common.hpp
  Engine/Core/Network.hpp
  Engine/Core/Network.cpp
  Engine/Core/Application.hpp
  Engine/Platform/SDLApplication.cpp
  Engine/Platform/EmscriptenApplication.cpp
)
add_library(Core STATIC ${CORE_SRCS})
source_group(Core FILES ${PLATFORM_SRCS})

set(EMSCRIPTEN_ASSET_PACK_FLAG "--preload-file ../Assets")
set(ENGINE_LIBS Core Graphics Math imgui ${LIBS})

add_executable(Inhabited Tests/Main.cpp)
target_link_libraries(Inhabited ${ENGINE_LIBS})

add_executable(MeshViewer Tools/MeshViewer.cpp)
target_link_libraries(MeshViewer ${ENGINE_LIBS})

add_executable(TexturePacker Tools/TexturePacker.cpp)
target_link_libraries(TexturePacker ${ENGINE_LIBS})

#Ensure EMCSCRIPTEN is given enough memory to work with...
if(EMSCRIPTEN)
  target_link_libraries(app "-s TOTAL_MEMORY=134217728")
  target_link_libraries(app ${EMSCRIPTEN_ASSET_PACK_FLAG})
endif()





# file(GLOB TOOLS_SRCS Tools/*.cpp)
# add_executable(RaptorTools ${TOOLS_SRCS})
# target_link_libraries(RaptorTools RaptorCore)
# set(CMAKE_LIBRARY_PATH ../lib/${CMAKE_BUILD_TYPE})
# target_link_libraries(RaptorTools ${RAPTOR_LIBS})
#
