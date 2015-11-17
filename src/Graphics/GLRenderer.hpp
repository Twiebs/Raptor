#ifndef RAPTOR_GLRENDERER_HPP
#define RAPTOR_GLRENDERER_HPP

#include <string>

#include <Math/Matrix4.hpp>
#include <Math/Vector2.hpp>
#include <Math/Vector3.hpp>

#include <Graphics/Lighting.hpp>

#include <Core/types.h>
#include <Core/config.h>
#include <Core/logging.h>

#include <GL/glew.h>

class GLDebugHelper {
public:
	GLDebugHelper(const char* functionName);
	~GLDebugHelper();

	static const char* GetCurrentFunctionName();
private:
	static const char* currentFunctionName;
	static const char* lastFunctionName;
};

#define GLDebugHere() GLDebugHelper generatedGLDebugHelper(__FUNCTION__)


void BindTexture2DToUnit(U32 textureUnit, U32 textureID);


//
//inline void BindTexture2DToUnit (GLuint textureID, U32 unitIndex);
//
//
//inline void BindTexture2DToUnit (GLuint textureID, U32 unitIndex) {
//	glActiveTexture(GL_TEXTURE0 + unitIndex);
//	glBindTexture(GL_TEXTURE_2D, textureID);
//}
//
//void EnableGLDebugMode();
//


void DEBUGRenderQuad();
void DEBUGRenderCube();
void GLBasicTriangleTest();
void GLBasicQuadTest();
void GLIndexedQuadTest();

void GLCheckErrors();

#endif //RAPTOR_GLRENDERER_HPP
