#pragma once

//NOTE this is fucking stupid!
#include <Core/Application.hpp>

#include <GL/glew.h>
#include <Math/Matrix4.hpp>
#include <Math/Vector3.hpp>
#include <Math/Vector2.hpp>

#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Texture.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>

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

	struct Vertex3D {
		Vector3 position;
		Vector3 normal;
		Vector2 texCoord;
		Vector3 tangent;
	};

	struct VertexBufferGroup {
		GLuint vertexArrayID = 0;
		GLuint vertexBufferID = 0;
		GLuint elementBufferID = 0;

		~VertexBufferGroup();
	};

	struct MeshData {
		U32 vertexCount, indexCount;
		Vertex3D* vertices;
		U32* indices;
		U8* memblock;
	};

	void ImportMeshData(MeshData& data, aiMesh* ai_mesh);
	void CreateVertexBuffersForMeshData(const MeshData& mesh, VertexBufferGroup& group);

	struct Material {
		GLuint diffuseMapID = 0;
		GLuint specularMapID = 0;
		GLuint normalMapID = 0;
		~Material();
	};

	void BindMaterial(const Material& material);
	void CalculateNormals(Vertex3D* vertices, U32 vertexCount, U32* indices, U32 indexCount);

	struct DebugModelData {
		std::vector<MeshData> meshes;
		std::vector<U32> meshMaterialIndex;
		std::vector<VertexBufferGroup> meshVertexBuffers;
		std::vector<Material> materials;

		void LoadFromFile(const std::string& filename);
	};

	void Draw(DebugModelData& model);

	struct Camera {
		float viewportWidth, viewportHeight;
		float yaw= -90, pitch = 0;
		float nearClip = 0.1f, farClip = 100.0f;

		Vector3 position;
		Vector3 front;
		Matrix4 view;
		Matrix4 projection;

		Camera();
		Camera(const Vector3& position, float viewportWidth, float viewportHeight);
	};

	void UpdateCamera(Camera& camera);
	void FPSCameraControlUpdate(Application* app, Camera& camera);

	inline void PushMatrix(U32 location, const Matrix4& matrix);
	inline void PushMatrix(U32 location, const Matrix4& matrix) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
	}

	struct PointLight {
		Vector3 position;
		Vector3 color;
		float linear;
		float quadratic;
		float radius;
	};

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

	void InitDepthShader(DepthShader& shader, U32 mapWidth, U32 mapHeight);
	GLuint CreateDepthFramebuffer(GLuint depthCubemap);
	GLuint CreateDepthCubemap(U32 textureWidth, U32 textureHeight);
	void BeginDepthShadingPass(DepthShader& shader);
	void EndDepthShadingPass();

	// Remove this stuff
	void DepthShadingPushLight(PointLight& light, DepthShader& shader);

	// =======================================
	//			Foward Shading
	// =======================================

	struct FowardShader {
		GLuint program;
	};

	void InitFowardShader(FowardShader& shader);
	void BeginFowardShadingPass(const FowardShader& shader, const DepthShader& depthShader, const Camera& camera);
	void EndFowardShadingPass();

	// =======================================
	//			Deferred Shading
	// =======================================

#if 1
	struct DeferredShader {
		GLuint gBuffer, renderBuffer;
		GLuint gPosition, gNormal, gColor;
		GLuint lightingPassProgram, geometeryPassProgram;
		GLuint quadVertexArray;
	};
#else
	struct DeferredShader {
		GLuint gBuffer;
		GLuint textures[3];
	};
#endif

	void InitDeferredShader(DeferredShader& shader, U32 screenWidth, U32 screenHeight);
	void RemoveDeferredShader(DeferredShader& shader);
	void BeginDeferredShadingGeometryPass(DeferredShader& shader, Camera& camera);
	void BeginDeferredShadingLightingPass(DeferredShader& shader, Camera& camera);
	void EndDeferredShadingGeometeryPass();
	void EndDeferredShadingLightingPass(DeferredShader& shader);

	// ======================================
	//		DEBUG stuff
	// ======================================

	void CreateDebugCube(GLuint* vertexArrayID, GLuint* vertexBufferID, float scale = 1.0f);
	void DrawDebugCube();

};