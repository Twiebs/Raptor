#include "GFX2D.hpp"
#include <Graphics/Render2D.hpp>
#include <Graphics/GLSLProgram.hpp>

struct GFX2DContext {
	IndexedDrawBuffer buffer;
	GLuint shaderProgramID;
	GLuint whiteTextureID;
	GLint projectionViewLoc;
	GLuint currentTextureID;
#ifdef DEBUG
	bool beginWasCalled = false;
#endif
} global_context;

void GFX2D::Init() {
	InitDrawBuffer(&global_context.buffer, 1 << 16);
	std::string vertexSource = R"(
	#version 100
	attribute vec2 position;
	attribute vec2 uv;
	attribute vec4 color;

	varying vec2 fragUV;
	varying vec4 fragColor;

	uniform mat4 projectionView;
	void main() {
		fragUV = uv;
		fragColor = color;
		gl_Position = projectionView * vec4(position, 0.0, 1.0);
	}
    )";

	std::string fragmentSource = R"(
	#version 100
	precision mediump float;

	varying vec2 fragUV;
	varying vec4 fragColor;

	uniform sampler2D sampler;

	void main() {
		gl_FragColor = fragColor * texture2D(sampler, fragUV);
		// gl_FragColor = fragColor;
	}
    )";

	global_context.shaderProgramID = CreateShaderFromSource(vertexSource.c_str(), fragmentSource.c_str());
	U8 whitePixel[4] = { 255, 255, 255, 255 };
	global_context.whiteTextureID = CreateTextureFromPixels(1, 1, whitePixel);
	global_context.currentTextureID = global_context.whiteTextureID;

	global_context.projectionViewLoc = GetUniformLocation(global_context.shaderProgramID, "projectionView");
	auto defaultMatrix = Matrix4::Identity();
	glUseProgram(global_context.shaderProgramID);
	glUniformMatrix4fv(global_context.projectionViewLoc, 1, GL_FALSE, &defaultMatrix[0][0]);
	glUniform1i(GetUniformLocation(global_context.shaderProgramID, "sampler"), 0);
	glUseProgram(0);
}

void GFX2D::Terminate() {

}

void GFX2D::Begin() {
	assert(global_context.shaderProgramID && "GFX2D::Init() must be called before using GFX2D::Begin()");
#ifdef DEBUG
	assert(!global_context.beginWasCalled);
	global_context.beginWasCalled = true;
#endif
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(global_context.shaderProgramID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, global_context.currentTextureID);
}

void GFX2D::SetProjectionMatrix(const Matrix4& projection) {
#ifdef DEBUG
	assert(global_context.beginWasCalled);
#endif
	glUniformMatrix4fv(global_context.projectionViewLoc, 1, GL_FALSE, &projection[0][0]);
}

void GFX2D::FillRect (F32 x, F32 y, F32 width, F32 height, const Color& color) {
	if (global_context.currentTextureID != global_context.whiteTextureID) {
		Draw(&global_context.buffer);
		glBindTexture(GL_TEXTURE_2D, global_context.whiteTextureID);
		global_context.currentTextureID = global_context.whiteTextureID;
	}

	PushRect(&global_context.buffer, x, y, width, height, color);
}

void GFX2D::Texture (U32 textureID, F32 x, F32 y, F32 width, F32 height, const Color& color) {
	if (textureID != global_context.currentTextureID) {
		Draw(&global_context.buffer);
		glBindTexture(GL_TEXTURE_2D, textureID);
		global_context.currentTextureID = textureID;
	}

	PushRect(&global_context.buffer, x, y, width, height, color);
}



void GFX2D::End() {
#ifdef DEBUG
	assert(global_context.beginWasCalled);
	global_context.beginWasCalled = false;
#endif
	Draw(&global_context.buffer);
	glBindVertexArray(0);
}
