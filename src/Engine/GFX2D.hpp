#ifndef RAPTOR_GFX2D_HPP
#define RAPTOR_GFX2D_HPP

#include <Math/Matrix4.hpp>
#include <Graphics/Color.hpp>

struct GFX2D {

static void Init();
static void Terminate();

static void Begin();
static void End();
static void SetProjectionMatrix(const Matrix4& projection);

static void FillRect (F32 x, F32 y, F32 width, F32 height, const Color& color = Color::WHITE);
static void Texture(U32 textureID, F32 x, F32 y, F32 width, F32 height, const Color& color = Color::WHITE);

private:
	GFX2D();
	~GFX2D();
};

#endif //RAPTOR_GFX2D_HPP
