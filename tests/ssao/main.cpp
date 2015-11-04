#include <Core/Platform.h>
#include <Graphics/Render3D.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/SSAO.hpp>

#include <Engine/Engine.hpp>
#include <Engine/Assets.hpp>

#include <Math/Math.hpp>

#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Raptor;

#define SHADER_DIRECTORY PROJECT_SOURCE_DIRECTORY "/tests/ssao/shaders/"
#define CORE_SHADER_DIRECTORY PROJECT_SOURCE_DIRECTORY "/src/shaders/"
#define CORE_SHADER(shadername) CORE_SHADER_DIRECTORY shadername


#define SHADER_FILE(filename) SHADER_DIRECTORY filename


void Draw (const Model& model) {
	glBindVertexArray(model.indexedVertexBuffer.vertexArrayID);
	size_t indexMemoryOffset = 0;
	for (U32 i = 0; i < model.meshCount; i++) {
		// BindMaterial(GetMaterial(model.meshMaterialHandles[i]));
		glDrawElements(GL_TRIANGLES, model.meshIndexCounts[i], GL_UNSIGNED_INT, (GLvoid*)indexMemoryOffset);
		indexMemoryOffset += model.meshIndexCounts[i] * sizeof(U32);
	}
	glBindVertexArray(0);
}



void DEBUGRenderQuad() {
	static GLuint quadVAO = 0;
	static GLuint quadVBO = 0;
	if (quadVAO == 0) {
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}


void DEBUGRenderCube() {
	static GLuint cubeVAO = 0;
	static GLuint cubeVBO = 0;
	if (cubeVAO == 0) {
		GLfloat vertices[] = {
			// Back face
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
															  // Front face
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
															   // Left face
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
															  // Right face
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
															 // Bottom face
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
																// Top face
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// Fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// Render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

static void RenderSSAOPass (SSAOBuffer* ssao, GBuffer* gbuffer, Camera* camera, const Shader& ssaoShader, const Shader& ssaoBlurShader) {
	// 2. Create SSAO texture
	glBindFramebuffer(GL_FRAMEBUFFER, ssao->occlusionFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(ssaoShader.id);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer->positionBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gbuffer->normalBuffer);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ssao->noiseTexture);

	Uniform::SetMatrix4(UniformLocation::PROJECTION_MATRIX, camera->projection);
	DEBUGRenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, ssao->blurFramebufffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(ssaoBlurShader.id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssao->colorbuffer);
	DEBUGRenderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void RenderDeferredLightingPass(GBuffer* gbuffer, SSAOBuffer* ssao, Shader& lightingShader, Camera* camera) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(lightingShader.id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer->positionBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gbuffer->normalBuffer);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gbuffer->colorBuffer);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ssao->blurTexture);

	// glm::vec3 lightPosView = glm::vec3(view * glm::vec4(lightPos, 1.0));
	// glUniform3fv(glGetUniformLocation(lightingShader.id, "light.Position"), 1, &lightPosView[0]);
	// glUniform3fv(glGetUniformLocation(lightingShader.id, "light.Color"), 1, &lightColor[0]);
	// Update attenuation parameters
	const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
	const GLfloat linear = 0.09;
	const GLfloat quadratic = 0.032;
	glUniform1f(glGetUniformLocation(lightingShader.id, "light.Linear"), linear);
	glUniform1f(glGetUniformLocation(lightingShader.id, "light.Quadratic"), quadratic);
	// glUniform1i(glGetUniformLocation(lightingShader.id, "draw_mode"), draw_mode);
	DEBUGRenderQuad();
}

static ShaderHandle CreateSSAOShader() {
	ShaderBuilder builder("GBuffer");
	builder.addSourceFile(VERTEX_SHADER, CORE_SHADER("vertex_screenquad.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, CORE_SHADER("SSAO.frag"));
	ShaderHandle result =LoadShader(builder);
	return result;
}

void RunSSAOTest() {
	int w, h;
	PlatformGetSize(&w, &h);

	Engine::Init();

	Camera camera(w, h, 45.0f, 0.1f, 50.0f);

	glViewport(0, 0, w, h);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Setup and compile our shaders
	ShaderBuilder builder("GBuffer");
	builder.addSourceFile(VERTEX_SHADER, CORE_SHADER("DeferredGenericMaterial.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, CORE_SHADER("DeferredGenericMaterial.frag"));
	auto geometryPassHandle = LoadShader(builder);

	Shader shaderGeometryPass = GetShader(geometryPassHandle);


	//Shader shaderSSAO = CreateShaderObject(SHADER_FILE("ssao.vs"), SHADER_FILE("ssao.frag"));
	ShaderHandle shaderSSAOHandle = CreateSSAOShader();
	Shader shaderSSAO = GetShader(shaderSSAOHandle);

	// Shader shaderGeometryPass = CreateShaderObject(CORE_SHADER("GBuffer.vert"), CORE_SHADER("GBuffer.frag"));
	Shader shaderLightingPass = CreateShaderObject(SHADER_FILE("ssao.vs"), SHADER_FILE("ssao_lighting.frag"));

	Shader shaderSSAOBlur = CreateShaderObject(SHADER_FILE("ssao.vs"), SHADER_FILE("ssao_blur.frag"));

	// Set samplers
	glUseProgram(shaderLightingPass.id);
	glUniform1i(glGetUniformLocation(shaderLightingPass.id, "gPositionDepth"), 0);
	glUniform1i(glGetUniformLocation(shaderLightingPass.id, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(shaderLightingPass.id, "gAlbedo"), 2);
	glUniform1i(glGetUniformLocation(shaderLightingPass.id, "ssao"), 3);
	glUseProgram(shaderSSAO.id);
	glUniform1i(glGetUniformLocation(shaderSSAO.id, "gPositionDepth"), 0);
	glUniform1i(glGetUniformLocation(shaderSSAO.id, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(shaderSSAO.id, "texNoise"), 2);

	// Objects
	ModelData data;
	ImportModelData(&data, ASSET_FILE("models/nanosuit/nanosuit.obj"));
	Model nanosuit = CreateModel(&data);

	// ModelHandle nanosuitHandle = LoadModel(ASSET_FILE("models/nanosuit/nanosuit.obj"));

	// Lights
	glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
	glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

	// Set up G-Buffer
	// 3 textures:
	// 1. Positions + depth (RGBA)
	// 2. Color (RGB) 
	// 3. Normals (RGB) 

	GBuffer gbuffer;
	SSAOBuffer ssaoBuffer;
	InitGBuffer(&gbuffer, w, h);
	InitSSAO(&ssaoBuffer, w, h);

	glUseProgram(shaderSSAO.id);
	SetSSAOUniforms(&ssaoBuffer, shaderSSAO);
	glUseProgram(0);

	//GLuint gBuffer;
	//glGenFramebuffers(1, &gBuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	//GLuint gPositionDepth, gNormal, gAlbedo;
	//// - Position + linear depth color buffer
	//glGenTextures(1, &gPositionDepth);
	//glBindTexture(GL_TEXTURE_2D, gPositionDepth);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
	//// - Normal color buffer
	//glGenTextures(1, &gNormal);
	//glBindTexture(GL_TEXTURE_2D, gNormal);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	//// - Albedo color buffer
	//glGenTextures(1, &gAlbedo);
	//glBindTexture(GL_TEXTURE_2D, gAlbedo);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
	//// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	//GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	//glDrawBuffers(3, attachments);
	//// - Create and attach depth buffer (renderbuffer)
	//GLuint rboDepth;
	//glGenRenderbuffers(1, &rboDepth);
	//glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	//// - Finally check if framebuffer is complete
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	std::cout << "GBuffer Framebuffer not complete!" << std::endl;



	// Also create framebuffer to hold SSAO processing stage 
	// GLuint ssaoFBO;

	//GLuint ssaoFBO;
	//GLuint ssaoColorBuffer;
	//glGenFramebuffers(1, &ssaoFBO);  
	//glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	//glGenTextures(1, &ssaoColorBuffer);
	//glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RGB, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	std::cout << "SSAO Framebuffer not complete!" << std::endl;




	//// Sample kernel
	//std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	//std::default_random_engine generator;
	//std::vector<glm::vec3> ssaoKernel;
	//for (GLuint i = 0; i < 64; ++i) {
	//	glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
	//	sample = glm::normalize(sample);
	//	sample *= randomFloats(generator);
	//	GLfloat scale = GLfloat(i) / 64.0;

	//	// Scale samples s.t. they're more aligned to center of kernel
	//	scale = Lerp(0.1f, 1.0f, scale * scale);
	//	sample *= scale;
	//	ssaoKernel.push_back(sample);
	//}

	//// Noise texture
	//std::vector<glm::vec3> ssaoNoise;
	//for (GLuint i = 0; i < 16; i++) {
	//	glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
	//	ssaoNoise.push_back(noise);
	//}
	//GLuint noiseTexture; glGenTextures(1, &noiseTexture);
	//glBindTexture(GL_TEXTURE_2D, noiseTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	auto to_glm = [](V3& v) -> glm::vec3 {
		return glm::vec3(v.x, v.y, v.z);
	};




	static int draw_mode;
	PlatformRun([&](F64 deltaTime) {
		FPSCameraControlUpdate(&camera);
		UpdateCamera(&camera);

		glm::mat4 projection = glm::perspective(45.0f, (float)w / (float)h, 0.1f, 50.0f);
		glm::mat4 view = glm::lookAt(to_glm(camera.position), to_glm(camera.position + camera.front), glm::vec3(0.0, 1.0f, 0.0));

		// 1. Geometry Pass: render scene's geometry/color data into gbuffer
		// glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.frameBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glm::mat4 projection = glm::perspective(camera.fov, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 50.0f);
		//glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model;
		glUseProgram(shaderGeometryPass.id);
		glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.id, "projectionMatrix"), 1, GL_FALSE, &camera.projection[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.id, "viewMatrix"), 1, GL_FALSE, &camera.view[0][0]);
		// Floor cube
		model = glm::translate(model, glm::vec3(0.0, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.id, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
		DEBUGRenderCube();
		// Nanosuit model on the floor
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0));
		model = glm::rotate(model, -90.0f, glm::vec3(1.0, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.id, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
		// auto& nanosuit = GetModel(nanosuitHandle);
		Draw(nanosuit);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		RenderSSAOPass(&ssaoBuffer, &gbuffer, &camera, shaderSSAO, shaderSSAOBlur);
		RenderDeferredLightingPass(&gbuffer, &ssaoBuffer, shaderLightingPass, &camera);


	});

	Engine::Terminate();
}

int main() {
	PlatformCreate("SSAO Test");
	RunSSAOTest();
	return 0;
}