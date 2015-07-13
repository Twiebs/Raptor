#pragma once
#include <vector>

#include <Core/Common.hpp>
#include <GL/glew.h>

#include <Utils/IO.hpp>
#include <Math/Vector2.hpp>

GLuint CreateArrayTexture2D(U32 width, U32 height, U32 layerCount, std::vector<std::string>& filenames);
GLuint CreateTextureFromPixels(U32 width, U32 height, U8* pixels);
Vector2 GetTexelCoords(U32 x, U32 y, U32 width, U32 height);
