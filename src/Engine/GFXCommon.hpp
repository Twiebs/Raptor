#pragma once

#include <Graphics/GLSLProgram.hpp>

// inline void BindShader (ShaderHandle shaderHandle);
inline void BindShader(const Shader& shader);

inline void BindShader(const Shader& shader) {
	glUseProgram(shader.id);
}