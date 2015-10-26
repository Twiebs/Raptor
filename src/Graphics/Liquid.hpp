#pragma once

#include <Graphics/GLRenderer.hpp>

#define LIQUID_PARAMETER_LOCATION 10
#define LIQUID_PARAMETER_COMPONENT_COUNT 8

struct LiquidRenderParameters {
	float amplitudes[LIQUID_PARAMETER_COMPONENT_COUNT];
	float wavelength[LIQUID_PARAMETER_COMPONENT_COUNT];
	float speeds[LIQUID_PARAMETER_COMPONENT_COUNT];
	Vector2 directions[LIQUID_PARAMETER_COMPONENT_COUNT];
};

void SetLiquidRenderSettingsUniforms(LiquidRenderParameters* params);