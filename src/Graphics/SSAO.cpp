#include "SSAO.hpp"

#include <Core/logging.h>
#include <Math/Random.hpp>
#include <Math/Math.hpp>
#include <Graphics/Uniform.hpp>

const U32 SSAOBuffer::KERNEL_SIZE_LOCATION = 3;
const U32 SSAOBuffer::KERNEL_RADIUS_LOCATION = 4;
const U32 SSAOBuffer::KERNEL_SAMPLE_LOCATION = 5;

void InitSSAO (SSAOBuffer* shader, U32 screenWidth, U32 screenHeight) {
	glGenFramebuffers(1, &shader->occlusionFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shader->occlusionFramebuffer);

	glGenTextures(1, &shader->colorbuffer);
	glBindTexture(GL_TEXTURE_2D, shader->colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shader->colorbuffer, 0);
	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG_ERROR("SSAO Frambuffer not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &shader->blurFramebufffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shader->blurFramebufffer);
	glGenTextures(1, &shader->blurTexture);
	glBindTexture(GL_TEXTURE_2D, shader->blurTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shader->blurTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//Create the Noise Texture
	Random rng(0);
	V3 noise[16];
	for (auto i = 0; i < 16; i++) { 
		noise[i] = Vector3(rng.Range(-1.0f, 1.0f), rng.Range(-1.0f, 1.0f), 0.0f);
		noise[i].Normalize();
	}

	glGenTextures(1, &shader->noiseTexture);
	glBindTexture(GL_TEXTURE_2D, shader->noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, noise);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	for (auto i = 0; i < SSAO_MAX_KERNEL_SIZE; i++) {
		shader->samples[i] = V3(rng.Range(-1.0f, 1.0f), rng.Range(-1.0f, 1.0f), rng.Range(0.0f, 1.0f));
		shader->samples[i].Normalize();
		shader->samples[i] *= rng.Range(0.0f, 1.0f);
		float scale = (float)i / 64.0f;
		scale = Lerp(0.1f, 1.0f, scale * scale);
		shader->samples[i] *= scale;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SetSSAOUniforms(SSAOBuffer* ssao, const Shader& shader) {
	Uniform::SetInt(SSAOBuffer::KERNEL_SIZE_LOCATION, ssao->kernelSize);
	Uniform::SetFloat(SSAOBuffer::KERNEL_RADIUS_LOCATION, ssao->kernelRadius);
	auto kernelSizeLoc = Uniform::GetLocation(shader.id, "uKernelSize");
	auto kernelRadiusLoc = Uniform::GetLocation(shader.id, "uKernelRadius");

	for (U32 i = 0; i < 64; i++) {
		auto location = GetUniformLocation(shader.id, ("samples[" + std::to_string(i) + "]").c_str());
		Uniform::SetVector3(location, ssao->samples[i]);
	}
}
