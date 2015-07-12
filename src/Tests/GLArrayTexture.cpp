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
GLuint gShaderProgramID;
GLuint gDebugProgramID;
DEBUGRenderGroup gRenderGroup;
DEBUGBufferGroup gBufferGroup;

GLuint projectionLoc;

GLuint CreateArrayTexture2D(U32 width, U32 height, U32 count, std::vector<std::string>& filenames) {
	GLuint textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA, width, height, count);

	for(auto i = 0; i < filenames.size(); i++) {
		auto pixmap = LoadPixmap(filenames[i]);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixmap->data);
	}
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	return textureID;
}

void MainLoop() {
	if(app.IsKeyDown(KEY_ESCAPE))	app.isRunning = false;
	glUseProgram(gShaderProgramID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, gTextureID);

	ArrayTextureVertex2D verts[4];
	verts[0] =  { Vector2(-0.75f, -0.75f), Vector3(0.0f, 0.0f, 1.0f) };
	verts[1] =  { Vector2(-0.55f, -0.75f), Vector3(1.0f, 0.0f, 1.0f) };
	verts[2] =  { Vector2(-0.5f, -0.5f), Vector3(1.0f, 1.0f, 1.0f) };
	verts[3] =  { Vector2(-0.75f, -0.5f), Vector3(0.0f, 1.0f, 1.0f) };
	DEBUGPushVertices(&gBufferGroup, verts, 4);
	DEBUGDrawGroup(&gBufferGroup);

	glUseProgram(gDebugProgramID);
	Matrix4 projection = Matrix4::Identity();
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

	DEBUGBindGroup(&gRenderGroup);
	DEBUGFillRect(&gRenderGroup, -0.5f, -0.5f, 1.0f, 1.0f, Color());
	DEBUGFlushGroup(&gRenderGroup);
}

int main() {
	app.Create("GLArrayTextureTest", 1280, 720, false);

	std::vector<std::string> textureFiles;
	// textureFiles.push_back(ASSET("textures/water.png"));
	// textureFiles.push_back(ASSET("textures/sand.png"));
	// gTextureID = CreateArrayTexture2D(64, 64, 2, textureFiles);
	// gShaderProgramID = DEBUGLoadShaderFromFile(ASSET("shaders/ArrayTexture.vert"), ASSET("shaders/ArrayTexture.frag"));
	gDebugProgramID = DEBUGLoadShaderFromFile(ASSET("shaders/Terrain2DVertex.glsl"), ASSET("shaders/Terrain2DFragment.glsl"));
	projectionLoc = glGetUniformLocation(gDebugProgramID, "projection");

	//
	// DEBUGCreateRenderGroup(&gRenderGroup, 1024);
	//
	// DEBUGCreateBufferGroup(&gBufferGroup, sizeof(Vertex2D), 1024);
	// DEBUGAddAttribute(&gBufferGroup, 0, GL_FLOAT, 2, sizeof(ArrayTextureVertex2D), offsetof(ArrayTextureVertex2D, position));
	// DEBUGAddAttribute(&gBufferGroup, 1, GL_FLOAT, 3, sizeof(ArrayTextureVertex2D), (void*)offsetof(ArrayTextureVertex2D, texCoord));

	app.Run(MainLoop);
}
