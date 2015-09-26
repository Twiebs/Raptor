

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
if(EMSCRIPTEN)
 # target_link_libraries(app "-s TOTAL_MEMORY=134217728")
  #target_link_libraries(app "--preload-file ../Assets")
endif()
