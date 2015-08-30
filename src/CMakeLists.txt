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
if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
else(EMSCRPITEN)
  add_definitions(-DPLATFORM_SDL)
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
    opengl32.lib
    glew32s.lib
	SOIL.lib

    SDL2.lib
    SDL2_image.lib
    SDL2_mixer.lib

    SDL2_net.lib
	assimpd.lib

	BulletCollision.lib
	BulletDynamics.lib
	LinearMath.lib
  )
endif(MSVC)

if(MSVC)
  file(GLOB RELEASE_LIBS ../lib/Release)
  file(GLOB DEBUG_LIBS ../lib/Debug)
endif(MSVC)

include_directories(../src)
include_directories(${INCS})

#Debug ouput the includes and libs that were found
message("Included directories: " ${INCS})
message("Using libaries: " ${LIBS})

#Setup the include directoriess

#This is to deal with freetypes sillyness...
#not only is it silly but windows and linux want it two different ways!
if(MSVC)
	include_directories(../include/freetype)
else()
	include_directories(/usr/include/freetype2)
endif()

set(CORE_SRCS
  Core/Common.hpp
  Core/Console.hpp
  Core/Console.cpp
  Core/Network.hpp
  Core/Network.cpp
  Core/Application.hpp
  Core/Audio.cpp
  Core/Audio.hpp
  Core/Platform.cpp
  Core/Platform.h
  Core/Tasks.hpp
  Core/Tasks.cpp
  Platform/SDLApplication.cpp
  Platform/EmscriptenApplication.cpp
)
source_group(Core FILES ${CORE_SRCS})

set(MATH_SRCS
  Math/RPhysics.cpp
  Math/RPhysics.hpp
  Math/Geometry2D.cpp
  Math/Geometry2D.hpp
  Math/Geometry3D.cpp
  Math/Geometry3D.hpp
  Math/Noise.cpp
  Math/Noise.hpp
  Math/Random.cpp
  Math/Random.hpp
  Math/Matrix4.cpp
  Math/Matrix4.hpp
  Math/Vector2.cpp
  Math/Vector2.hpp
  Math/Vector3.hpp
  Math/Vector3.cpp
)
source_group(Math FILES ${MATH_SRCS})

set(GRAPHICS_SRCS
	Graphics/Texture.cpp
	Graphics/Texture.hpp
	Graphics/Color.hpp
	Graphics/DEBUGRenderer.cpp
	Graphics/DEBUGRenderer.hpp
	Graphics/GLSLProgram.cpp
	Graphics/GLSLProgram.hpp
	Graphics/Render3D.hpp
	Graphics/Render3D.cpp
	Graphics/ModelData.cpp
	Graphics/ModelData.hpp
	Graphics/imgui.cpp
	Graphics/imgui_draw.cpp
	Graphics/imgui_demo.cpp
	Graphics/imgui_user.cpp
	Graphics/imgui_user.h
)
source_group(Graphics FILES ${GRAPHICS_SRCS})

SET(ENGINE_SRCS
  Engine/Assets.cpp
  Engine/Assets.hpp
  Engine/Debug.cpp
  Engine/Debug.hpp
  Engine/World.cpp
  Engine/World.hpp
  Engine/Editor.cpp
  Engine/Editor.hpp
)
source_group(Engine FILES ${ENGINE_SRCS})

add_library(RaptorEngine STATIC ${CORE_SRCS} ${MATH_SRCS} ${GRAPHICS_SRCS} ${ENGINE_SRCS})

add_executable(WorldTest Tests/WorldTest.cpp )
target_link_libraries(WorldTest RaptorEngine ${LIBS})

add_executable(MeshViewer Tools/MeshViewer.cpp)
target_link_libraries(MeshViewer RaptorEngine ${LIBS})

add_executable(ModelViewer Tools/ModelViewer.cpp)
target_link_libraries(ModelViewer RaptorEngine ${LIBS})

add_executable(Manhattan Manhattan/Main.cpp)
target_link_libraries(Manhattan RaptorEngine ${LIBS})

set(EMSCRIPTEN_ASSET_PACK_FLAG "--preload-file ../Assets")


# add_executable(Inhabited Tests/Main.cpp)
# target_link_libraries(Inhabited RaptorEngine ${LIBS})

# add_executable(WorldTest Tests/WorldTest.cpp)
# target_link_libraries(WorldTest RaptorEngine ${LIBS})
# target_link_libraries(WorldTest debug ${DEBUG_LIBS})
# target_link_libraries(WorldTest optimized ${RELEASE_LIBS})

# add_executable(MeshViewer Tools/MeshViewer.cpp)
# target_link_libraries(MeshViewer ${ENGINE_LIBS})
# target_link_libraries(MeshViewer debug ${DEBUG_LIBS})
# target_link_libraries(MeshViewer optimized ${RELEASE_LIBS})
#
# add_executable(TexturePacker Tools/TexturePacker.cpp)
# target_link_libraries(TexturePacker ${ENGINE_LIBS})

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
