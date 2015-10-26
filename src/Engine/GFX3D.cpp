#include "GFX3D.hpp"

#include <Graphics/GLRenderer.hpp>
#include <Graphics/Render3D.hpp>
#include <Engine/Assets.hpp>

void ApplyDebugRenderSettings (const Shader& shader, const DebugRenderSettings& settings);

using namespace Raptor;

struct GFX3DRenderContext {
	Camera* camera;
	bool beginWasCalled = false;
	GLuint currentShaderID = 0;

	SceneLighting lighting;
	DebugRenderSettings debug_settings;
} static context;


struct DrawList {
	std::vector<Matrix4> modelMatrices;
	std::vector<Mesh> meshes;
};


void GFX3D::Init() {
}

void GFX3D::Terminate() {

}

void AddLight(const DirectionalLight& light) {
	context.lighting.directionalLight.push_back((DirectionalLight*)&light);
}

void AddLight(const PointLight& light) {
	context.lighting.pointLights.push_back(light);
}

// TODO keep information in the context if the 
// renderer needs to clear aditional render buffers!

// We probably want to allow different cameras
// and also have somthign called GFX3D::BeginPass(PassSettings& settings, Camera&)
// that alows specification of things like DEPTH_TEST / blending / render mode / whatever else

ShaderHandle CreateNormalDebugShader() {
	ShaderBuilder builder("normal_visualizer");
	builder.addSourceFile(VERTEX_SHADER,   SHADER_FILE("debug_normals.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("debug_normals.frag"));
	builder.addSourceFile(GEOMETRY_SHADER, SHADER_FILE("debug_normals.geom"));
	ShaderHandle normalVisualizerSH = LoadShader(builder);
	return normalVisualizerSH;
}


static void SetSceneLightingUniforms(SceneLighting* lighting,  GLuint currentShaderID) {
	for (U32 i = 0; i < lighting->directionalLight.size(); i++) {
		Uniform::SetDirectionalLight(lighting->directionalLight[i], i, currentShaderID);
	}
}

void GFX3D::BeginFrame(Camera& camera, FrameParameters* params) {
	assert(context.camera == nullptr && "The GFX3D::EndFrame() was not called before calling GFX3D::BeginFrame() again!");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glClearColor(params->clearColor.x, params->clearColor.y, params->clearColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UpdateCamera(&camera);
	context.camera = &camera;
	glPolygonMode(GL_FRONT_AND_BACK, context.debug_settings.enableWireframe ? GL_LINE : GL_FILL);
}

void GFX3D::Begin(const Shader& shader) {
	assert(context.camera != nullptr && "Begin frame must be called with a specified projection camera!");
	assert(!context.beginWasCalled && "End must be called before calling begin once again!");
	GFX3D::BindShader(shader);
	SetCameraUniforms(*context.camera);
	SetSceneLightingUniforms(&context.lighting, context.currentShaderID);
	SetMatrix4Uniform(MODEL_MATRIX_LOCATION, Matrix4::Identity());	// Make sure the model matrix is always valid

	ApplyDebugRenderSettings(shader, context.debug_settings);
}

void GFX3D::BindShader(const Shader& shader) {
	context.currentShaderID = shader.id;
	glUseProgram(shader.id);
	SetCameraUniforms(*context.camera);
}

void GFX3D::End() {
	context.beginWasCalled = false;
	context.currentShaderID = 0;
}

void GFX3D::DrawMesh(const Mesh& mesh) {
	assert(context.currentShaderID != 0);
	PushMatrix(MODEL_MATRIX_LOCATION, Matrix4::Identity());
	glBindVertexArray(mesh.vertexArrayID);
	glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
}

void GFX3D::DrawMesh(const Mesh& mesh, const Vector3& position) {
	assert(context.currentShaderID != 0);
	PushMatrix(MODEL_MATRIX_LOCATION, Matrix4::Translate(position));
	glBindVertexArray(mesh.vertexArrayID);
	glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
}



void GFX3D::DrawModel(const Model& model) {
	assert(context.currentShaderID != 0);
	PushMatrix(MODEL_MATRIX_LOCATION, Matrix4::Identity());

	glBindVertexArray(model.indexedVertexBuffer.vertexArrayID);
	U8 indexMemoryOffset = 0;
	for (U32 i = 0; i < model.meshCount; i++) {
		glDrawElements(GL_TRIANGLES, model.meshIndexCounts[i], GL_UNSIGNED_INT, (void*)indexMemoryOffset);
		indexMemoryOffset += sizeof(U32) * model.meshIndexCounts[i];
	}
}

void GFX3D::DrawModel(const Model& model, const Vector3& position) {
	assert(context.currentShaderID != 0);
	PushMatrix(MODEL_MATRIX_LOCATION, Matrix4::Translate(position));

	glBindVertexArray(model.indexedVertexBuffer.vertexArrayID);
	U8 indexMemoryOffset = 0;
	for (U32 i = 0; i < model.meshCount; i++) {
		glDrawElements(GL_TRIANGLES, model.meshIndexCounts[i], GL_UNSIGNED_INT, (void*)indexMemoryOffset);
		indexMemoryOffset += sizeof(U32) * model.meshIndexCounts[i];
	}
}



void GFX3D::EndFrame() {
	context.camera = nullptr;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

DebugRenderSettings* GetDebugRenderSettings() {
	return &context.debug_settings;
}

void ApplyDebugRenderSettings (const Shader& shader, const DebugRenderSettings& settings) {
	auto disableNormalMapLocation = GetUniformLocation(shader.id, "debug_settings.disableNormalMaps");
	glUniform1i(disableNormalMapLocation, settings.disableNormalMaps);
}

//void ApplyDebugRenderSettings (const DebugRenderSettings& settings) {
//	auto& manifest = GetGlobalAssetManifest();
//	for (auto& shader : manifest.shaderPrograms) {
//
//
//		BindShader(shader);
//
//	}
//}