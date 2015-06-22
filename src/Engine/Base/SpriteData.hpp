#pragma once

#include <Core/Common.hpp>
#include <Graphics/Color.hpp>

//Some data representing a sprite
struct SpriteData {
      float32 x, y;
      float32 width, height;
      uint32 textureID;
      Color color;
}
