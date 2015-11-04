#pragma once

#include <Engine/Assets.hpp>
#include <Graphics/GLSLProgram.hpp>

#define SSAO_MAX_KERNEL_SIZE 128

struct SSAOBuffer {
	GLuint occlusionFramebuffer;
	GLuint blurFramebufffer;
	GLuint blurTexture;
	GLuint colorbuffer;
	GLuint noiseTexture;
	V3 samples[SSAO_MAX_KERNEL_SIZE];
	U32 kernelSize = 64;
	float kernelRadius = 1.0;

	static const U32 KERNEL_SIZE_LOCATION;
	static const U32 KERNEL_RADIUS_LOCATION;
	static const U32 KERNEL_SAMPLE_LOCATION;
};

void InitSSAO(SSAOBuffer* shader, U32 screenWidth, U32 screenHeight);
void SetSSAOUniforms(SSAOBuffer* ssao, const Shader& shader);
