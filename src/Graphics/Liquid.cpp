#include "Liquid.hpp"

#include <Graphics/Uniform.hpp>

void SetLiquidRenderSettingsUniforms (LiquidRenderParameters* settings) {
	Uniform::SetFloats(LIQUID_PARAMETER_LOCATION + 1, settings->amplitudes, LIQUID_PARAMETER_COMPONENT_COUNT);
	Uniform::SetFloats((LIQUID_PARAMETER_LOCATION + 1) + (LIQUID_PARAMETER_COMPONENT_COUNT * 0),
		settings->amplitudes, LIQUID_PARAMETER_COMPONENT_COUNT);
	Uniform::SetFloats((LIQUID_PARAMETER_LOCATION + 1) + (LIQUID_PARAMETER_COMPONENT_COUNT * 1),
		settings->wavelength, LIQUID_PARAMETER_COMPONENT_COUNT);
	Uniform::SetFloats((LIQUID_PARAMETER_LOCATION + 1) + (LIQUID_PARAMETER_COMPONENT_COUNT * 2),
		settings->speeds, LIQUID_PARAMETER_COMPONENT_COUNT);
	Uniform::SetVector2((LIQUID_PARAMETER_LOCATION + 1) + (LIQUID_PARAMETER_COMPONENT_COUNT * 3),
		settings->directions, LIQUID_PARAMETER_COMPONENT_COUNT);
}