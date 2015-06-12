#include "IRenderer.h"


IRenderer::IRenderer(Camera* camera) {
	this->camera = camera;
}


IRenderer::~IRenderer() {
}

void IRenderer::SetCamera(Camera* camera){
	this->camera = camera;
}


void IRenderer::ToggleWireframeEnabled() {
	SetWireframeEnabled(!wireframeEnabled);
}