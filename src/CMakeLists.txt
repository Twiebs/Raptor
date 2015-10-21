include_directories(../src)
include_directories(${INCS})

set(CORE_SRCS
  Core/Common.hpp
  Core/Console.hpp
  Core/Console.cpp
  #Core/Network.hpp
  #Core/Network.cpp
  Core/Audio.cpp
  Core/Audio.hpp
  Core/Platform.cpp
  Core/Platform.h
  Core/Tasks.hpp
  Core/Tasks.cpp

    Utils/utils.cpp
    Utils/utils.hpp
)
source_group(Core FILES ${CORE_SRCS})

set(MATH_SRCS
    Math/Procedural3D.cpp
    Math/Procedural3D.hpp
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
    Graphics/Color.cpp
	Graphics/Color.hpp
	Graphics/GLSLProgram.cpp
	Graphics/GLSLProgram.hpp
    Graphics/GLRenderer.cpp
    Graphics/GLRenderer.hpp
	Graphics/Render3D.hpp
	Graphics/Render3D.cpp
    Graphics/Render2D.hpp
    Graphics/Render2D.cpp
    Graphics/Lighting.cpp
    Graphics/Lighting.hpp
	Graphics/ModelData.cpp
	Graphics/ModelData.hpp
        Graphics/Mesh.cpp
        Graphics/Mesh.hpp
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
    Engine/World.cpp
    Engine/World.hpp
    Engine/Physics2D.cpp
    Engine/Physics2D.hpp
    Engine/Physics3D.cpp
    Engine/Physics3D.hpp
    Engine/GFX2D.cpp
    Engine/GFX2D.hpp
    Engine/GFX3D.cpp
    Engine/GFX3D.hpp

    Engine/ECSManager.cpp
    Engine/ECSManager.hpp

    Engine/Debug.cpp
    Engine/Debug.hpp
    Engine/Editor.cpp
    Engine/Editor.hpp
)
source_group(Engine FILES ${ENGINE_SRCS})

add_library(RaptorEngine STATIC ${CORE_SRCS} ${MATH_SRCS} ${GRAPHICS_SRCS} ${ENGINE_SRCS})

set(BOX2D_VERSION 2.3.1)
set(BOX2D_BUILD_STATIC true)
add_subdirectory(Box2D)

if(EMSCRIPTEN)
 # target_link_libraries(app "-s TOTAL_MEMORY=134217728")
  #target_link_libraries(app "--preload-file ../Assets")
endif()
