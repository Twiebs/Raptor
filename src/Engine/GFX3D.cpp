#include "GFX3D.hpp"

#include <Graphics/Uniform.hpp>
#include <Graphics/Skybox.hpp>
#include <Graphics/GLRenderer.hpp>
#include <Graphics/SSAO.hpp>
#include <Graphics/Render3D.hpp>
#include <Engine/Assets.hpp>

#include <Core/Platform.h>

const U32 SceneLighting::DIRECTIONAL_LIGHT_COUNT_LOCATION = 4;
const U32 SceneLighting::POINT_LIGHT_COUNT_LOCATION = 5;

using namespace Raptor;

global_variable Material DEFAULT_MATERIAL;

static void DEBUGRenderQuad();
static void DEBUGRenderCube();

struct ModelDrawCommand {
	Matrix4 transformMatrix;
	Model* model;
	Shader* shader;
};

struct MeshShadingInfo {
	Material* material;
	Shader* shader;
};

struct MeshGeometryInfo {
	Matrix4 modelMatrix;
	Mesh* mesh;
};

struct MeshDrawList {
	std::vector<MeshShadingInfo> meshShadingInfo;
	std::vector<MeshGeometryInfo> meshGeometryInfo;
	std::vector<ModelDrawCommand> modelInfos;
};


struct DeferedShadingPass {
	GBuffer gbuffer;
	ShaderHandle lightingShader;
};

struct ShadowRenderPass {
	U32 mapWidth, mapHeight;
	F32 nearClip, farClip;
	GLuint depthFramebuffer;
	GLuint depthCubemap;
	Matrix4 projectionMatrix;
	Matrix4 shadowMatrices[6];
	GLint shadowMatrixLocations[6];
	GLint nearLocation, farLocation;
	GLint lightPositionLocation;
	ShaderHandle cubemapShader;
};

struct GFX3DRenderContext {
	Camera* camera;
	bool beginWasCalled = false;

	Shader* boundShader = nullptr;
	Shader* shaderForCommand = nullptr;


	SceneLighting lighting;
	DebugRenderSettings* settings;

	GFX3DRenderMode mode;

	Material* currentMaterial;

	MeshDrawList drawList;

	GLuint screenQuadVertexArray;

	ShaderHandle pointLightDebugShaderHandle;

	// Skybox
	ShaderHandle skyboxShaderHandle;
	GLuint skyboxVAO, skyboxVBO;

	DeferedShadingPass deferedShadingInfo;
	ShadowRenderPass shadowPass;

	SSAOBuffer ssao;
	ShaderHandle ssaoShaderHandle;
	ShaderHandle ssaoBlurShaderHandle;
};

static bool IsValid(const Shader& shader);

global_variable GFX3DRenderContext context;

static void BindShader(const Shader& shader);

static void UnbindAll();

static void DrawMeshGeometry(MeshGeometryInfo* geometry) {
	assert(context.boundShader != nullptr && "A shader must be bound when drawing!");
	Uniform::SetMatrix4(UniformLocation::MODEL_MATRIX, geometry->modelMatrix);
	glBindVertexArray(geometry->mesh->vertexArrayID);
	glDrawElements(GL_TRIANGLES, geometry->mesh->indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

static void DrawMeshWithShading(MeshGeometryInfo* geometry, MeshShadingInfo* shading) {
	assert(context.boundShader == nullptr && "Mesh shading uses its own material shader!");
	BindShader(*shading->shader);
	BindMaterial(*shading->material);
	Uniform::SetMatrix4(UniformLocation::MODEL_MATRIX, geometry->modelMatrix);

	glBindVertexArray(geometry->mesh->vertexArrayID);
	glDrawElements(GL_TRIANGLES, geometry->mesh->indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	UnbindAll();
}

static void DrawModelGeometry(ModelDrawCommand* cmd) {
	assert(context.boundShader != nullptr && "A shader must be bound when drawing scene geometry");
	Uniform::SetMatrix4(UniformLocation::MODEL_MATRIX, cmd->transformMatrix);
	
	glBindVertexArray(cmd->model->indexedVertexBuffer.vertexArrayID);
	size_t indexMemoryOffset = 0;
	for (U32 i = 0; i < cmd->model->meshCount; i++) {
		glDrawElements(GL_TRIANGLES, cmd->model->meshIndexCounts[i], GL_UNSIGNED_INT, (GLvoid*)indexMemoryOffset);
		indexMemoryOffset += cmd->model->meshIndexCounts[i] * sizeof(U32);
	}
	glBindVertexArray(0);
}

static void DrawModelWithShading(ModelDrawCommand* cmd) {
	assert(context.boundShader == nullptr && "Shaders must be unbound when renderering material shaders");
	BindShader(*cmd->shader);
	Uniform::SetMatrix4(UniformLocation::MODEL_MATRIX, cmd->transformMatrix);

	glBindVertexArray(cmd->model->indexedVertexBuffer.vertexArrayID);
	size_t indexMemoryOffset = 0;
	for (U32 i = 0; i < cmd->model->meshCount; i++) {
		BindMaterial(GetMaterial(cmd->model->meshMaterialHandles[i]));
		glDrawElements(GL_TRIANGLES, cmd->model->meshIndexCounts[i], GL_UNSIGNED_INT, (GLvoid*)indexMemoryOffset);
		indexMemoryOffset += cmd->model->meshIndexCounts[i] * sizeof(U32);
	}
	glBindVertexArray(0);
	UnbindAll();
}

static void DrawAllGeometry(MeshDrawList* list) {
	assert(context.boundShader != nullptr && "A shader must be bound when drawing scene geometry");
	
	for (U32 i = 0; i < list->meshGeometryInfo.size(); i++) {
		DrawMeshGeometry(&list->meshGeometryInfo[i]);
	}

	for (U32 i = 0; i < list->modelInfos.size(); i++) {
		ModelDrawCommand* cmd = &list->modelInfos[i];
		DrawModelGeometry(cmd);
	}
}

static void DrawAllWithShading(MeshDrawList* list) {
	assert(context.boundShader == nullptr && "All shaders must be unbound");

	for (U32 i = 0; i < list->meshGeometryInfo.size(); i++) {
		DrawMeshWithShading(&list->meshGeometryInfo[i], &list->meshShadingInfo[i]);
	}

	for (U32 i = 0; i < list->modelInfos.size(); i++) {
		ModelDrawCommand* cmd = &list->modelInfos[i];
		DrawModelWithShading(cmd);
	}
}

//static void ExecuteDrawCommand(MeshDrawCommand* cmd);
//static void ExecuteDrawCommand(ModelDrawCommand* cmd);
//
//static void ExecuteDrawCommand(ModelDrawCommand* cmd) {
//	BindShader(*cmd->shader);
//	Uniform::SetMatrix4(MODEL_MATRIX_LOCATION, cmd->transformMatrix);
//
//	size_t indexMemoryOffset = 0;
//	for (U32 i = 0; i < cmd->model->meshCount; i++) {
//		auto& material = GetMaterial(cmd->model->meshMaterialHandles[i]);
//		BindMaterial(material);
//		glBindVertexArray(cmd->model->indexedVertexBuffer.vertexArrayID);
//		glDrawElements(GL_TRIANGLES, cmd->model->meshIndexCounts[i], GL_UNSIGNED_INT, (void*)indexMemoryOffset);
//		indexMemoryOffset += sizeof(U32) * cmd->model->meshIndexCounts[i];
//	}
//}
//
//static void ExecuteDrawCommand(MeshDrawCommand* cmd) {
//	BindShader(*cmd->shader);
//	BindMaterial(*cmd->material);
//	glBindVertexArray(cmd->mesh->vertexArrayID);
//	glDrawElements(GL_TRIANGLES, cmd->mesh->indexCount, GL_UNSIGNED_INT, 0);
//}




GLuint DebugGetShadowMap() {
	return context.shadowPass.depthCubemap;
}


static void SetSceneLightingUniforms(SceneLighting* lighting, GLuint currentShaderID) {
	assert((lighting->directionalLight.size() > 0 || lighting->pointLights.size() > 0) &&
		"You are trying to draw a scene without any lights!");

	Uniform::SetInt(SceneLighting::DIRECTIONAL_LIGHT_COUNT_LOCATION, lighting->directionalLight.size());
	Uniform::SetInt(SceneLighting::POINT_LIGHT_COUNT_LOCATION, lighting->pointLights.size());
	for (U32 i = 0; i < lighting->directionalLight.size(); i++)
		Uniform::SetDirectionalLight(lighting->directionalLight[i], i, currentShaderID);
	for (U32 i = 0; i < lighting->pointLights.size(); i++)
		Uniform::SetPointLight(lighting->pointLights[i], i, currentShaderID);
}

static void ApplyDebugRenderSettingsToActiveShader (DebugRenderSettings* settings) {
	assert(context.boundShader != nullptr);
	if (settings == nullptr) return;

	context.mode = settings->renderMode;

	auto disableNormalsLoc = Uniform::GetLocationNoErrorCheck(context.boundShader->id, "GDebug.disableNormalMaps");
	auto drawModeLoc = Uniform::GetLocationNoErrorCheck(context.boundShader->id, "GDebug.drawMode");
	Uniform::SetInt(disableNormalsLoc, settings->disableNormalMaps);
	Uniform::SetInt(drawModeLoc, (int)settings->deferredDrawMode);
}

static void ApplyDebugRenderSettingsToRenderer (DebugRenderSettings* settings) {
	if (settings == nullptr) return;

	if (settings->disableGammaCorrection) {
		glEnable(GL_FRAMEBUFFER_SRGB);
	} else {
		glDisable(GL_FRAMEBUFFER_SRGB);
	}
}


static ShaderHandle CreateNormalDebugShader() {
	ShaderBuilder builder("normal_visualizer");
	builder.addSourceFile(VERTEX_SHADER, SHADER_FILE("debug_normals.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("debug_normals.frag"));
	builder.addSourceFile(GEOMETRY_SHADER, SHADER_FILE("debug_normals.geom"));
	ShaderHandle normalVisualizerSH = LoadShader(builder);
	return normalVisualizerSH;
}

static inline ShaderHandle CreateDeferedLightingShader() {
	ShaderBuilder builder("GFX3D_DeferedLightingShader");
	builder.addSourceFile(VERTEX_SHADER, SHADER_FILE("screen_quad.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("DeferredShading.frag"));
	ShaderHandle result = LoadShader(builder);
	return result;
}

static ShaderHandle CreateShadowCubemapShader() {
	ShaderBuilder builder("ShadowCubemap");
	builder.addSourceFile(VERTEX_SHADER, SHADER_FILE("DepthCubemap.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("DepthCubemap.frag"));
	builder.addSourceFile(GEOMETRY_SHADER, SHADER_FILE("DepthCubemap.geom"));
	auto result = LoadShader(builder);
	return result;
}

static ShaderHandle CreateSkyboxShader() {
	ShaderBuilder skyboxShaderBuilder("Skybox");
	skyboxShaderBuilder.addSourceFile(VERTEX_SHADER, SHADER_FILE("skybox.vert"));
	skyboxShaderBuilder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("skybox.frag"));
	return LoadShader(skyboxShaderBuilder);
}

static ShaderHandle CreatePointLightDebugShader() {
	ShaderBuilder builder("PointLightDebug");
	builder.addSourceFile(VERTEX_SHADER, SHADER_FILE("DebugLights.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("DebugLights.frag"));
	return LoadShader(builder);
}

static ShaderHandle CreateSSAOShader() {
	ShaderBuilder builder ("SSAO");
	builder.addSourceFile(VERTEX_SHADER, SHADER_FILE("vertex_screenquad.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("SSAO.frag"));
	return LoadShader(builder);
}

static ShaderHandle CreateSSAOBlurShader() {
	ShaderBuilder builder("SSAOBlur");
	builder.addSourceFile(VERTEX_SHADER, SHADER_FILE("vertex_screenquad.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("SSAOBlur.frag"));
	return LoadShader(builder);
}


void InitDeferedShading (DeferedShadingPass* shader) {
	int w, h;
	PlatformGetSize(&w, &h);
	InitGBuffer(&shader->gbuffer, w, h);
	// shader->gbufferShader = CreateGBufferShader();
	shader->lightingShader = CreateDeferedLightingShader();
}


// ========================================================================================
// ========================================================================================
// ========================================================================================

static void RenderDebugPointLights (SceneLighting* lighting) {
	static const U32 LIGHT_COLOR_LOCATION = 3;

	auto& pointLightDebugShader = GetShader(context.pointLightDebugShaderHandle);
	BindShader(pointLightDebugShader);
	SetCameraUniforms(*context.camera);

	for (U32 i = 0; i < lighting->pointLights.size(); i++) {
		PointLight* light = lighting->pointLights[i];
		Uniform::SetVector3(LIGHT_COLOR_LOCATION, light->color);
		Uniform::SetMatrix4(UniformLocation::MODEL_MATRIX, Matrix4::Translate(context.lighting.pointLights[i]->position));
		DEBUGRenderCube();
	}

	UnbindAll();
}


static void RenderFowardPass() {
	auto& skyboxShader = GetShader(context.skyboxShaderHandle);
	glUseProgram(skyboxShader.id);
	SetCameraUniforms(*context.camera);
	Uniform::SetMatrix4(UniformLocation::MODEL_MATRIX, Matrix4::Scale(V3(100)));
	Uniform::SetMatrix4(UniformLocation::VIEW_MATRIX, context.camera->view * Matrix4::Translate(context.camera->position));
	glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, context.shadowPass.depthCubemap);
	glBindVertexArray(context.skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	UnbindAll();
}

static void RenderDeferredGeometryPass (GBuffer* gbuffer, MeshDrawList* drawList) {
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	int width, height;
	PlatformGetSize(&width, &height);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer->frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawAllWithShading(drawList);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void RenderSSAOPass (SSAOBuffer* ssao, GBuffer* gbuffer, Camera* camera, const Shader& ssaoShader, const Shader& ssaoBlurShader) {
	glBindFramebuffer(GL_FRAMEBUFFER, ssao->occlusionFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(ssaoShader.id);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer->positionBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gbuffer->normalBuffer);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ssao->noiseTexture);

	Uniform::SetMatrix4(UniformLocation::VIEW_MATRIX, camera->view);
	Uniform::SetMatrix4(UniformLocation::PROJECTION_MATRIX, camera->projection);
	for (GLuint i = 0; i < ssao->kernelSize; ++i) {
		glUniform3fv(SSAOBuffer::KERNEL_SAMPLE_LOCATION, ssao->kernelSize, &ssao->samples[i].x);
	}

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


static void RenderDeferedLightingPass (DeferedShadingPass* shadingPass, const Shader& shader) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	SetSceneLightingUniforms(&context.lighting, shader.id);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadingPass->gbuffer.positionBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadingPass->gbuffer.normalBuffer);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadingPass->gbuffer.colorBuffer);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, context.shadowPass.depthCubemap);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, context.ssao.colorbuffer);

	glBindVertexArray(context.screenQuadVertexArray);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	int width, height;
	PlatformGetSize(&width, &height);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, shadingPass->gbuffer.frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void RenderScene() {
	RenderDeferredGeometryPass(&context.deferedShadingInfo.gbuffer, &context.drawList);

	auto& ssaoShader = GetShader(context.ssaoShaderHandle);
	auto& ssaoBlurShader = GetShader(context.ssaoBlurShaderHandle);
	// RenderSSAOPass(&context.ssao, &context.deferedShadingInfo.gbuffer, context.camera, ssaoShader, ssaoBlurShader);

	auto& deferredLightingShader = GetShader(context.deferedShadingInfo.lightingShader);
	RenderDeferedLightingPass(&context.deferedShadingInfo, deferredLightingShader);

	// RenderFowardPass();
	UnbindAll();
	RenderDebugPointLights(&context.lighting);
}

// ========================================================================================
// ========================================================================================
// ========================================================================================

static void InitShadowRenderPass (ShadowRenderPass* shadowPass, U32 width, U32 height) 
{
	shadowPass->mapWidth = width;
	shadowPass->mapHeight = height;
	shadowPass->depthCubemap = CreateDepthCubemap(width, height);
	shadowPass->depthFramebuffer = CreateDepthFramebuffer(shadowPass->depthCubemap);
	shadowPass->nearClip = 1.0f;
	shadowPass->farClip = 25.0f;

	float aspectRatio = (float)width / (float)height;
	shadowPass->projectionMatrix = Matrix4::Perspective(90.0f, aspectRatio, 1.0f, 25.0f);
	shadowPass->cubemapShader = CreateShadowCubemapShader();

	auto& cubemapShader = GetShader(shadowPass->cubemapShader);
	for (U32 i = 0; i < 6; i++) {
		shadowPass->shadowMatrixLocations[i] = Uniform::GetLocation(cubemapShader.id, "shadowMatrices[" + std::to_string(i) + "]");
	}

	shadowPass->nearLocation = Uniform::GetLocation(cubemapShader.id, "nearClip");
	shadowPass->farLocation = Uniform::GetLocation(cubemapShader.id, "farPlane");
	shadowPass->lightPositionLocation = Uniform::GetLocation(cubemapShader.id, "lightPos");
}


// HACK
// Need ability to generate multiple shadow maps for the lights in the area
// and create these properties for them when they are rendered!
static void SetShadowRenderPassUniforms (ShadowRenderPass* shadowPass) 
{
	PointLight* light = context.lighting.pointLights[0];

	shadowPass->shadowMatrices[0] = shadowPass->projectionMatrix * Matrix4::LookAt(light->position, light->position + V3( 1.0,  0.0,  0.0),  V3(0.0, -1.0,  0.0));
	shadowPass->shadowMatrices[1] = shadowPass->projectionMatrix * Matrix4::LookAt(light->position, light->position + V3(-1.0,  0.0,  0.0),  V3(0.0, -1.0,  0.0));
	shadowPass->shadowMatrices[2] = shadowPass->projectionMatrix * Matrix4::LookAt(light->position, light->position + V3( 0.0,  1.0,  0.0),  V3(0.0,  0.0,  1.0));
	shadowPass->shadowMatrices[3] = shadowPass->projectionMatrix * Matrix4::LookAt(light->position, light->position + V3( 0.0, -1.0,  0.0),  V3(0.0,  0.0, -1.0));
	shadowPass->shadowMatrices[4] = shadowPass->projectionMatrix * Matrix4::LookAt(light->position, light->position + V3( 0.0,  0.0,  1.0),  V3(0.0, -1.0,  0.0));
	shadowPass->shadowMatrices[5] = shadowPass->projectionMatrix * Matrix4::LookAt(light->position, light->position + V3( 0.0,  0.0, -1.0),  V3(0.0, -1.0,  0.0));
	Uniform::SetMatrix4(shadowPass->shadowMatrixLocations[0], shadowPass->shadowMatrices[0]);
	Uniform::SetMatrix4(shadowPass->shadowMatrixLocations[1], shadowPass->shadowMatrices[1]);
	Uniform::SetMatrix4(shadowPass->shadowMatrixLocations[2], shadowPass->shadowMatrices[2]);
	Uniform::SetMatrix4(shadowPass->shadowMatrixLocations[3], shadowPass->shadowMatrices[3]);
	Uniform::SetMatrix4(shadowPass->shadowMatrixLocations[4], shadowPass->shadowMatrices[4]);
	Uniform::SetMatrix4(shadowPass->shadowMatrixLocations[5], shadowPass->shadowMatrices[5]);

	Uniform::SetVector3(shadowPass->lightPositionLocation, light->position);
	Uniform::SetFloat(shadowPass->nearLocation, shadowPass->nearClip);
	Uniform::SetFloat(shadowPass->farLocation, shadowPass->farClip);
}



static void DrawShadowRenderPass (ShadowRenderPass* shadowPass, MeshDrawList* drawList) {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, shadowPass->mapWidth, shadowPass->mapHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowPass->depthFramebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	BindShader(GetShader(shadowPass->cubemapShader));
	SetShadowRenderPassUniforms(shadowPass);
	DrawAllGeometry(&context.drawList);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void GFX3D::Init() {
	int w, h;
	PlatformGetSize(&w, &h);
	InitDeferedShading(&context.deferedShadingInfo);
	context.mode = GFX3D_DEFERED;

	context.skyboxShaderHandle = CreateSkyboxShader();
	context.pointLightDebugShaderHandle = CreatePointLightDebugShader();

	InitSSAO(&context.ssao, w, h);
	// context.ssaoBlurShaderHandle = CreateSSAOBlurShader();

	context.ssaoShaderHandle = CreateSSAOShader();
	auto& ssaoShader = GetShader(context.ssaoShaderHandle);
	glUseProgram(ssaoShader.id);
	SetSSAOUniforms(&context.ssao, ssaoShader);
	glUseProgram(0);


	CreateSkyboxVertices(&context.skyboxVAO, &context.skyboxVBO);

	glGenVertexArrays(1, &context.screenQuadVertexArray);

	InitShadowRenderPass(&context.shadowPass, 1024, 1024);
}

void GFX3D::Terminate() {

}

void AddLight (const DirectionalLight& light) {
	SceneLighting* lighting = &context.lighting;
	assert(lighting->directionalLight.size() + 1 < lighting->maxDirectionalLightCount
		&& "The current shaders are not configured for directional lights of this size");
	context.lighting.directionalLight.push_back((DirectionalLight*)&light);
}

void AddLight (PointLight* light) {
	SceneLighting* lighting = &context.lighting;
	assert(lighting->pointLights.size() < lighting->maxPointLightCount
		&& "The current shaders are not configured for pointLights lights of this size");
	lighting->pointLights.push_back(light);
}

void GFX3D::SetCamera (Camera* camera) {
	UpdateCamera(camera);
	context.camera = camera;
}

void GFX3D::SetFrameParameters (FrameParameters* params) {



}

void GFX3D::BeginFrame (FrameParameters* params, Camera* camera) {
	assert(context.camera == nullptr && "The GFX3D::EndFrame() was not called before calling GFX3D::BeginFrame() again!");

	glClearColor(params->clearColor.x, params->clearColor.y, params->clearColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	UpdateCamera(camera);
	context.camera = camera;

	ApplyDebugRenderSettingsToRenderer(context.settings);
}

static void UnbindAll() {
	glUseProgram(0);
	glBindVertexArray(0);
	BindTexture2DToUnit(0, 0);
	BindTexture2DToUnit(0, 1);
	BindTexture2DToUnit(0, 2);

	context.currentMaterial = nullptr;
	context.boundShader = nullptr;
}



void GFX3D::EndFrame() {
	assert(context.beginWasCalled == false && "End was not called on the current shading pass");
	// assert(context.shaderForCommand == 0 && "The shader was never cleared!");
	// DrawShadowRenderPass(&context.shadowPass, &context.drawList);


	RenderScene();
	

	context.camera = nullptr;

	context.drawList.meshGeometryInfo.clear();
	context.drawList.meshShadingInfo.clear();
	context.drawList.modelInfos.clear();
}

void BindShader (const Shader& shader) {
	assert(IsValid(shader));

	context.boundShader = (Shader*)&shader;
	glUseProgram(shader.id);
	SetCameraUniforms(*context.camera);
	SetMatrix4Uniform(MODEL_MATRIX_LOCATION, Matrix4::Identity());	// Make sure the model matrix is always valid
	ApplyDebugRenderSettingsToActiveShader(context.settings);
}

void GFX3D::Begin (const Shader& shader) {
	assert(context.camera != nullptr && "Begin frame must be called with a specified projection camera!");
	assert(context.beginWasCalled == false && "End must be called before calling begin once again!");
	assert(context.shaderForCommand == 0 && "The last shader was not unbound!");

	context.beginWasCalled = true;
	context.shaderForCommand = (Shader*)&shader;
	context.currentMaterial = &DEFAULT_MATERIAL;
}

static bool IsValid (const Shader& shader) {
	bool result = shader.id > 0;
	return result;
}

void GFX3D::SetShader(const Shader& shader) {
	assert(IsValid(shader));
	context.shaderForCommand = (Shader*)&shader;
}

void GFX3D::End (const Shader& shader) {
	assert(context.beginWasCalled && "End was called without begin!");
	assert(context.shaderForCommand != nullptr && "There was no shader bound!");
	assert(context.shaderForCommand->id == shader.id
		&& "The shader used in this pass is not The same as the currently bound shader!");

	context.beginWasCalled = false;
	context.currentMaterial = nullptr;
	context.shaderForCommand = nullptr;
}

void GFX3D::AddMesh (const Mesh& mesh, const Vector3& position, const Vector3& rotation,const Vector3& scale) {
	assert(context.shaderForCommand != nullptr && "This mesh requires a shader to be drawn with");

	MeshGeometryInfo geometry = { };
	geometry.modelMatrix = Matrix4::Transform(position, rotation, scale);
	geometry.mesh = (Mesh*)&mesh;
	MeshShadingInfo shading = { };
	shading.material = context.currentMaterial;
	shading.shader = context.shaderForCommand;

	context.drawList.meshGeometryInfo.emplace_back(geometry);
	context.drawList.meshShadingInfo.emplace_back(shading);
}


void GFX3D::AddModel (const Model& model, const Vector3& position) {
	assert(context.shaderForCommand != nullptr);

	ModelDrawCommand cmd = { };
	cmd.transformMatrix = Matrix4::Translate(position);
	cmd.model = (Model*)&model;
	cmd.shader = context.shaderForCommand;
	context.drawList.modelInfos.emplace_back(cmd);
}


void GFX3D::SetDebugRenderSettings(DebugRenderSettings* settings) {
	context.settings = settings;
}

void GFX3D::SetMaterial (Material* material) {
	context.currentMaterial = material;
}