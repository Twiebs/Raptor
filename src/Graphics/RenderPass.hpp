#include <Core/types.h>
#include <GL/glew.h>

#include <Engine/Assets.hpp>
#include <Graphics/GLSLProgram.hpp>
#include <Math/Matrix4.hpp>

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
