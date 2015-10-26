#pragma once
#include <GL/glew.h>
#include <Math/Matrix4.hpp>
#include <Math/Vector3.hpp>
#include <Math/Vector2.hpp>

#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/ModelData.hpp>
#include <Graphics/Lighting.hpp>

#define MODEL_LOCATION 0
#define VIEW_LOCATION 1
#define PROJECTION_LOCATION 2

namespace Raptor {

	struct Transform3D {
		Vector3 translation;
		Vector3 rotation;
		Vector3 scale;
		Transform3D();
	};

	Matrix4 TransformToMatrix(const Transform3D& transform);

	void BindMaterial(const Material& material);

	void Draw(DebugModelData& model);

	struct Camera {
		float viewportWidth = 1280, viewportHeight = 720;
		float yaw= -90, pitch = 0;
		float nearClip = 0.1f, farClip = 100.0f;

		Vector3 position;
		Vector3 front;
		Matrix4 view;
		Matrix4 projection;
		Camera();
		Camera(const Vector3& position, float viewportWidth, float viewportHeight);
	};

	void UpdateCamera(Camera* camera);
	void FPSCameraControlUpdate(Camera* camera);

    inline void SetModelMatrix(Matrix4* matrix);
    inline void SetModelMatrix(const Vector3& position, const Vector3& rotation, const Vector3& scale);
    inline void SetModelMatrix(const Vector3& position, const Vector3& rotation, const Vector3& scale) {
        auto modelMatrix = Matrix4::Transform(position, rotation, scale);
        glUniformMatrix4fv(MODEL_LOCATION, 1, GL_FALSE, &modelMatrix[0][0]);
    }

	inline void PushMatrix(U32 location, const Matrix4& matrix);
	inline void PushMatrix(U32 location, const Matrix4& matrix) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
	}


	//TODO should remove need to pass shader...
	//Or even the index
	void PushLight(const PointLight& light, U32 index, GLuint shader);


	// =======================================
	//			Depth Shading
	// =======================================

	struct DepthShader {
		GLuint program;
		GLuint depth_map;
		GLuint framebuffer;
		U32 mapWidth, mapHeight;
		float nearPlane, farPlane;
		Matrix4 shadowProjectionMatrix;
	};

	void InitDepthShader(DepthShader* shader, U32 mapWidth, U32 mapHeight);
	GLuint CreateDepthFramebuffer(GLuint depthCubemap);
	GLuint CreateDepthCubemap(U32 textureWidth, U32 textureHeight);
	void BeginDepthShadingPass(DepthShader* shader);
	void EndDepthShadingPass();

	// Remove this stuff
	void DepthShadingPushLight(PointLight& light, DepthShader* shader);

	// =======================================
	//			Foward Shading
	// =======================================

	struct FowardShader {
		GLuint program;
	};

	void InitFowardShader(FowardShader* shader);
	void BeginFowardShadingPass(FowardShader* shader, DepthShader* depthShader, Camera* camera);
	void EndFowardShadingPass();

	// =======================================
	//			Deferred Shading
	// =======================================

	struct GBuffer {
		GLuint frameBuffer;
		GLuint renderBuffer;
		GLuint positionBuffer;
		GLuint normalBuffer;
		GLuint colorBuffer;
	};

	void InitGBuffer(GBuffer* buffer, U32 screenWidth, U32 screenHeight);
	void ReleaseGBuffer(GBuffer* buffer);

	struct DeferredShader {
		GLuint gBuffer, renderBuffer;
		GLuint gPosition, gNormal, gColor;
		GLuint lightingPassProgram, geometeryPassProgram;
		GLuint quadVertexArray;
	};

	void InitDeferredShader(DeferredShader* shader, U32 screenWidth, U32 screenHeight);
	void DestroyDeferredShader(DeferredShader* shader);
	void BeginDeferredShadingGeometryPass(DeferredShader* shader, Camera* camera);
	void BeginDeferredShadingLightingPass(DeferredShader* shader, Camera* camera);
	void EndDeferredShadingGeometeryPass();
	void EndDeferredShadingLightingPass(DeferredShader* shader);

	// ======================================
	//		DEBUG stuff
	// ======================================

	void CreateDebugCube(GLuint* vertexArrayID, GLuint* vertexBufferID, float scale = 1.0f);
	void DrawDebugCube();
};
