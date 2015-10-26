#ifndef RAPTOR_GFX3D_HPP
#define RAPTOR_GFX3D_HPP

#include <Engine/GFXCommon.hpp>
#include <Engine/Assets.hpp>

#include <Math/Matrix4.hpp>

#include <Graphics/Render3D.hpp>

#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>

struct FrameParameters {
	Vector3 clearColor;
};

struct DebugRenderSettings {
	bool disableNormalMaps = false;
	bool enableWireframe   = false;

	ShaderHandle normalVisualizer;
};

ShaderHandle CreateNormalDebugShader();

struct SceneLighting {
	std::vector<DirectionalLight*> directionalLight;
	std::vector<PointLight> pointLights;
};

void AddLight(const DirectionalLight& light);
void AddLight(const PointLight& light);


using namespace Raptor;

#define MODEL_MATRIX_LOCATION 0
#define VIEW_MATRIX_LOCATION 1
#define PROJECTION_MATRIX_LOCATION 2
#define CAMERA_POSITION_LOCATION 3

inline void SetVector3Uniform (GLint location, const Vector3& vector);
inline void SetCameraUniforms (const Camera& camera);
inline void SetMatrix4Uniform (U32 matrixLocation, const Matrix4& matrix);

inline void SetVector3Uniform (GLint location, const Vector3& vector) {
	glUniform3fv(location, 1, &vector.x);
}

inline void SetCameraUniforms (const Camera& camera) {
	SetVector3Uniform(CAMERA_POSITION_LOCATION, camera.position);
	SetMatrix4Uniform(VIEW_MATRIX_LOCATION, camera.view);
	SetMatrix4Uniform(PROJECTION_MATRIX_LOCATION, camera.projection);
}

inline void SetMatrix4Uniform (U32 matrixLocation, const Matrix4& matrix) {
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &matrix[0][0]);
}

enum GFX3D_RENDER_MODE {
	GFX3D_FOWARD,
	GFX3D_DEFERED,
};

DebugRenderSettings* GetDebugRenderSettings();

namespace GFX3D {

void Init();
void Terminate();

void BeginFrame(Camera& camera, FrameParameters* params);
void Begin (const Shader& shader);

void DrawMesh(const Mesh& mesh);
void DrawMesh(const Mesh& mesh, const Vector3& position);

void DrawModel(const Model& model);
void DrawModel(const Model& model, const Vector3& position);

void BindShader(const Shader& shader);
void End();
void EndFrame();

void SetProjectionMatrix (const Matrix4& matrix);

};

#endif // RAPTOR_GFX3D_HPP
