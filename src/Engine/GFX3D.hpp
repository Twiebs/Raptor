#ifndef RAPTOR_GFX3D_HPP
#define RAPTOR_GFX3D_HPP

#include <Engine/GFXCommon.hpp>
#include <Engine/Assets.hpp>
#include <Engine/Terrain.hpp>

#include <Math/Matrix4.hpp>

#include <Graphics/GLRenderer.hpp>
#include <Graphics/Render3D.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/Uniform.hpp>

struct FrameParameters {
	Vector3 clearColor = Vector3(0.0f, 0.0f, 0.0f);
};


GLuint DebugGetShadowMap();

//enum DebugDeferedDrawMode {
//	DRAWMODE_ALL,
//	DRAWMODE_POSITION,
//	DRAWMODE_NORMAL,
//	DRAWMODE_COLOR,
//	DRAWMODE_SPECULAR
//};

struct DebugRenderSettings {
	int deferredDrawMode = 0;

	bool disableNormalMaps = false;
	bool enableWireframe   = false;
	bool disableGammaCorrection = false;

	ShaderHandle normalVisualizer;
};

ShaderHandle CreateNormalDebugShader();

struct SceneLighting {
	std::vector<DirectionalLight*> directionalLight;
	std::vector<PointLight*> pointLights;

	U32 maxPointLightCount = 64;
	U32 maxDirectionalLightCount = 64;

	static const U32 DIRECTIONAL_LIGHT_COUNT_LOCATION;
	static const U32 POINT_LIGHT_COUNT_LOCATION;
};

void AddLight(const DirectionalLight& light);
void AddLight(PointLight* light);


using namespace Raptor;

#define CAMERA_POSITION_LOCATION 3

inline void SetCameraUniforms (const Camera& camera);

inline void SetCameraUniforms (const Camera& camera) {
	Uniform::SetMatrix4(UniformLocation::VIEW_MATRIX, camera.view);
	Uniform::SetMatrix4(UniformLocation::PROJECTION_MATRIX, camera.projection);
	Uniform::SetVector3(CAMERA_POSITION_LOCATION, camera.position);
}

inline void DrawScreenQuad (GLuint quadShaderID) {
	static GLuint vao = 0;
	if (!vao) glGenVertexArrays(1, &vao);
	glUseProgram(quadShaderID);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SetGammaCorrectionActive(bool flag);

namespace GFX3D {

void Init();
void Terminate();

void BeginFrame(FrameParameters* params, Camera* camera);
void EndFrame();

 // void Begin(const Shader& shader);
 // void End(const Shader& shader);

// void BindShader(const Shader& shader);
void SetDebugRenderSettings(DebugRenderSettings* settings);
void SetMaterialShader(const Shader& shader);
void SetMaterial(Material* material);
void SetCamera(Camera* camera);
void SetFrameParameters(FrameParameters* params);

void AddMesh(const Mesh& mesh, const Vector3& position = Vector3(0.0f), const Vector3& rotation = Vector3(0.0f), const Vector3& scale = Vector3(1.0f));
void AddModel(const Model& model, const Vector3& position = Vector3(0.0f));
void DrawTerrain(TerrainManager* terrain);

void RectLine(const V3& from, const V3& to, const Color& color);
void Line(const V3& from, const V3& to, const Color& color);

};

#endif // RAPTOR_GFX3D_HPP
