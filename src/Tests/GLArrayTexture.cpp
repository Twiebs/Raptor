#include <Application/Application.hpp>
#include <GL/glew.h>
#include <Utils/IO.hpp>

#include <Graphics/DEBUGRenderer.hpp>

Application app;

struct ArrayTextureVertex2D {
	Vector2 position;
	Vector3 texCoord;
};

GLuint gTextureID;
GLuint gNullTextureID;
GLuint gShaderProgramID;
GLuint gDebugProgramID;
DEBUGRenderGroup gRenderGroup;
DEBUGBufferGroup gBufferGroup;

DEBUGBufferGroup gTestGroup;

GLuint projectionLoc;

GLuint CreateArrayTexture2D(U32 width, U32 height, U32 layerCount, std::vector<std::string>& filenames) {
	GLuint textureID = 0;
	GLuint mipLevelCount = 1;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
	//Allocate the storage.
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, width, height, layerCount);
	//Upload pixel data.
	//The first 0 refers to the mipmap level (level 0, since there's only 1)
	//The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
	//The final 0 refers to the layer index offset (we start from index 0 and have 2 levels).
	//Altogether you can specify a 3D box subset of the overall texture, but only one mip level at a time.

	for (auto i = 0; i < layerCount; i++) {
		auto pixmap = LoadPixmap(filenames[i]);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixmap->data);
		delete pixmap;
	}

	//Always set reasonable texture parameters
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return textureID;
}

void MainLoop() {
	if(app.IsKeyDown(KEY_ESCAPE))	app.isRunning = false;

	glUseProgram(gShaderProgramID);
	GLuint samplerLoc = glGetUniformLocation(gShaderProgramID, "sampler");
	glUniform1i(samplerLoc, 0);

	DEBUGBindGroup(&gBufferGroup);
	ArrayTextureVertex2D verts[4];
	verts[0] =  { Vector2(-0.75f, -0.75f), Vector3(0.0f, 0.0f, 0.0f) };
	verts[1] =  { Vector2(-0.5f, -0.75f), Vector3(1.0f, 0.0f, 0.0f) };
	verts[2] =  { Vector2(-0.5f, -0.5f), Vector3(1.0f, 1.0f, 1.0f) };
	verts[3] =  { Vector2(-0.75f, -0.5f), Vector3(0.0f, 1.0f, 1.0f) };
	DEBUGPushVertices(&gBufferGroup, verts, 4);

	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D_ARRAY, gTextureID);
	glBindTexture(GL_TEXTURE_2D, gNullTextureID);
	DEBUGDrawGroup(&gBufferGroup);

	//glUseProgram(gDebugProgramID);
	//Matrix4 projection = Matrix4::Identity();
	//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

	//Vert verts[4];
	//verts[0] =  { Vector2(-0.75f, -0.75f), Vector2(0.0f, 0.0f), Color() };
	//verts[1] =  { Vector2(-0.5f, -0.75f), Vector2(1.0f, 0.0f), Color() };
	//verts[2] = { Vector2(-0.5f, -0.5f), Vector2(1.0f, 1.0f), Color() };
	//verts[3] =  { Vector2(-0.75f, -0.5f), Vector2(0.0f, 1.0f), Color() };

	//DEBUGBindGroup(&gTestGroup);
	//DEBUGPushVertices(&gTestGroup, verts, 4);
	//DEBUGDrawGroup(&gTestGroup);

	//DEBUGBindGroup(&gRenderGroup);
	//DEBUGPushVertices(&gRenderGroup, verts, 4);
	////DEBUGFillRect(&gRenderGroup, -0.5f, -0.5f, 1.0f, 1.0f, Color());
	//DEBUGFlushGroup(&gRenderGroup);
}

int main() {
	app.Create("GLArrayTextureTest", 1280, 720, false);

	std::vector<std::string> textureFiles;
	 textureFiles.push_back(ASSET("textures/water.png"));
	 textureFiles.push_back(ASSET("textures/null.png"));
	 gTextureID = CreateArrayTexture2D(128, 128, 2, textureFiles);
	// gTextureID = DEUBGCreateTexture2D();
	 gShaderProgramID = DEBUGLoadShaderFromFile(ASSET("shaders/ArrayTexture.vert"), ASSET("shaders/ArrayTexture.frag"));
	gDebugProgramID = DEBUGLoadShaderFromFile(ASSET("shaders/Terrain2DVertex.glsl"), ASSET("shaders/Terrain2DFragment.glsl"));
	projectionLoc = glGetUniformLocation(gDebugProgramID, "projection");

	gNullTextureID = DEBUGLoadTexture(ASSET("textures/null.png"));

	//
	DEBUGCreateRenderGroup(&gRenderGroup, 1024);
	//
	 DEBUGCreateBufferGroup(&gBufferGroup, sizeof(ArrayTextureVertex2D), 1024);
	 DEBUGAddAttribute(&gBufferGroup, 0, 2, GL_FLOAT, sizeof(ArrayTextureVertex2D), (void*)offsetof(ArrayTextureVertex2D, position));
	 DEBUGAddAttribute(&gBufferGroup, 1, 3, GL_FLOAT, sizeof(ArrayTextureVertex2D), (void*)offsetof(ArrayTextureVertex2D, texCoord));

	 DEBUGCreateBufferGroup(&gTestGroup, sizeof(Vert), 1024);
	 DEBUGAddAttribute(&gTestGroup, 0, 2, GL_FLOAT, sizeof(Vert), (GLvoid*)offsetof(Vert, position));
	 DEBUGAddAttribute(&gTestGroup, 1, 2, GL_FLOAT, sizeof(Vert), (GLvoid*)offsetof(Vert, uv));
	 DEBUGAddAttribute(&gTestGroup, 2, 4, GL_FLOAT, sizeof(Vert), (GLvoid*)offsetof(Vert, color));

	app.Run(MainLoop);
}
