#include "GFX3D.hpp"
#include <Graphics/Render3D.hpp>

using namespace Raptor;

struct GFX3DRenderContext {
	FowardShader shader;
} static context;

void GFX3D::Init() {
	InitFowardShader(&context.shader);
}

void GFX3D::Terminate() {

}

void GFX3D::Begin() {

}

void GFX3D::End() {

}

