#include <string>
#include <sstream>
#include <malloc.h>

#include <Application/Application.hpp>
#include <GL/glew.h>
#include <Utils/IO.hpp>

#include <Core/Common.hpp>
#include <Core/InputListener.hpp>

#include <ECS/ECSManager.hpp>
#include <Base/Transform2D.hpp>
#include <Base/RenderSystem2D.hpp>
#include <Base/PhysicsSystem2D.hpp>

#include <Graphics/Color.hpp>

#include <Math/MathUtils.hpp>
#include <Math/Matrix4.hpp>
#include <Math/Noise.hpp>

#include <imgui.h>
#include <Platform/ImGui.hpp>


Application app;
ImGuiContext gImGuiContext;

struct DEBUGRenderContext {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;
	GLuint shaderProgramID;

	GLuint currentTextureID;
	uint32 currentVertexCount;
	uint32 maxVertexCount;

	Matrix4 projection;
	GLuint projectionUniformLocation;
	GLuint isTextureUniformLocation;
	GLuint isWaterUniformLocation;
	GLuint waveAngleUniformLocation;
};


DEBUGRenderContext gRenderContext;

Transform2D gCameraTransform;
Transform2D gPlayerTransform;

const uint32 MAP_WIDTH = 512;
const uint32 MAP_WIDTH_PLUS_ONE = 513;
const uint32 MAP_HEIGHT = 512;
const uint32 MAP_HEIGHT_PLUS_ONE = 513;
float32* gHeightmap;
Color* gTilemap;

GLuint vertexMemorySize;
GLuint textureID;
GLSLProgram* shader;

ECSManager* ecs;
PhysicsSystem2D* physics2D;

struct Vert {
	Vector2 position;
	Vector2 uv;
	Color color;
};

void DEBUGFlushContext(DEBUGRenderContext* context) {
	glUseProgram(context->shaderProgramID);
	if(context->currentTextureID != 0) {
		glUniform1i(context->isTextureUniformLocation, 1);
	} else {
		glUniform1i(context->isTextureUniformLocation, 0);
	}

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, context->currentTextureID);

	glBindVertexArray(context->vertexArrayID);
	glDrawElements(GL_TRIANGLES, (context->currentVertexCount / 4) * 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	context->currentVertexCount = 0;
}


void DEBUGPushVertices(DEBUGRenderContext* context, Vert* vertices, uint32 count) {
	if(context->currentVertexCount + count > context->maxVertexCount)
		DEBUGFlushContext(context);

	glBindBuffer(GL_ARRAY_BUFFER, context->vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vert) * context->currentVertexCount, sizeof(Vert) * count, (GLvoid*)vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	context->currentVertexCount += count;
}

void DEBUGDrawTexture(GLuint textureID, float x, float y, float width, float height, Color color, DEBUGRenderContext* context) {
	if(context->currentTextureID != textureID) {
		DEBUGFlushContext(context);
		context->currentTextureID = textureID;
	}

	Vert verts[4];
	verts[0] = Vert { Vector2(x, y), Vector2(0.0f, 0.0f), color };
	verts[1] = Vert { Vector2(x + width, y), Vector2(1.0f, 0.0f), color };
	verts[2] = Vert { Vector2(x + width, y + height), Vector2(1.0f, 1.0f), color };
	verts[3] = Vert { Vector2(x, y + height), Vector2(0.0f, 1.0f), color };
	DEBUGPushVertices(context, verts, 4);
}

void DEBUGFillRect(float32 x, float32 y, float32 width, float32 height, Color color, DEBUGRenderContext* context) {
	if(context->currentTextureID != 0) {
		DEBUGFlushContext(context);
		context->currentTextureID = 0;
	}

	Vert verts[4];
	verts[0] = Vert { Vector2(x, y), Vector2(0.0f, 0.0f), color };
	verts[1] = Vert { Vector2(x + width, y), Vector2(1.0f, 0.0f), color };
	verts[2] = Vert { Vector2(x + width, y + height), Vector2(1.0f, 1.0f), color };
	verts[3] = Vert { Vector2(x, y + height), Vector2(0.0f, 1.0f), color };
	DEBUGPushVertices(context, verts, 4);
}


void DEBUGInitRenderer(DEBUGRenderContext* context, uint32 maxVertexCount) {
	const char* vertexShaderSource =
	"attribute vec2 position;"
	"attribute vec2 uv;"
	"attribute vec4 color;"

	"varying vec2 fragUV;"
	"varying vec4 fragColor;"

	"uniform mat4 projection;"
	"uniform float waveAngle;"
	"uniform bool isWater;"

	"void main() {"
		"fragUV = uv;"
		"fragColor = color;"
		"vec2 wavePos = vec2(position.x + sin(waveAngle + position.x + position.y), position.y + cos(waveAngle + position.x + position.y));"
		"gl_Position = projection * vec4(isWater ? wavePos : position.xy, 0.0, 1.0);"
	"}";

	const char* fragmentShaderSource =
	"precision mediump float;"
	"varying vec2 fragUV;"
	"varying vec4 fragColor;"
	"uniform sampler2D sampler;"
	"uniform bool isTexture;"
	"void main() {"
		"gl_FragColor = isTexture ? texture2D(sampler, fragUV) : fragColor;"
	"}";

	glGenVertexArrays(1, &context->vertexArrayID);
	glBindVertexArray(context->vertexArrayID);

	glGenBuffers(1, &context->vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, context->vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vert) * maxVertexCount, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, position));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, uv));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, color));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//TODO add facilities to obtain a block of temporary memory
	//TODO look into mapping the address of the element buffer into our address space and
	//Manually pushing the indices to it!
	uint32 indexCount = (maxVertexCount / 4) * 6;
	uint32* indices = new uint32[indexCount];
	for(uint32 i = 0, v = 0; i < indexCount; i += 6, v += 4) {
		indices[i + 0] = v + 0;
		indices[i + 1] = v + 3;
		indices[i + 2] = v + 2;
		indices[i + 3] = v + 0;
		indices[i + 4] = v + 2;
		indices[i + 5] = v + 1;
	}

	glGenBuffers(1, &context->elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * indexCount, indices, GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
	delete[] indices;

	context->currentVertexCount = 0;
	context->maxVertexCount = maxVertexCount;
	context->shaderProgramID = DEBUGLoadShaderFromSource(vertexShaderSource, fragmentShaderSource);
	context->projectionUniformLocation = glGetUniformLocation(context->shaderProgramID, "projection");
	context->isTextureUniformLocation = glGetUniformLocation(context->shaderProgramID, "isTexture");
	context->isWaterUniformLocation = glGetUniformLocation(context->shaderProgramID, "isWater");
	context->waveAngleUniformLocation = glGetUniformLocation(context->shaderProgramID, "waveAngle");
}

void DEBUGSetProjectionMatrix(DEBUGRenderContext* context, Matrix4& projection) {
	glUseProgram(context->shaderProgramID);
	glUniformMatrix4fv(context->projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);
}

Matrix4 TransformToOrtho(Transform2D& transform, float32 zoom) {
	float32 halfWidth = transform.size.x * 0.5f * zoom;
	float32 halfHeight = transform.size.y * 0.5f * zoom;
	return Matrix4::Ortho(
			(transform.position.x - halfWidth),
			(transform.position.x + halfWidth),
			(transform.position.y - halfHeight),
			(transform.position.y + halfHeight),
			1.0f
		);
}

Color GetColor(uint32 x, uint32 y) {
	static float32 colorThresholds[] {
		0.7,
		0.65f,
		0.5f,
		0.35f,
		0.25f,
		0.15f,
		-0.15f,
		-1.0f
	};
	static Color colors[] {
		Color(0.20f, 0.20f, 0.20f, 1.0f),
		Color(0.31f, 0.25f, 0.19f, 1.0f),
		Color(0.26f, 0.25f, 0.13f, 1.0f),
		Color(0.25f, 0.21f, 0.13f, 1.0f),
		Color(0.29f, 0.21f, 0.13f, 1.0f),
		Color(0.80f, 0.60f, 0.29f, 1.0f),
		Color(0.69f, 0.54f, 0.39f, 1.0f),
		Color(0.38f, 0.54f, 0.67f, 1.0f)
	};

	uint32 index = (y * MAP_WIDTH) + x;
	for(uint32 i = 0; i < 8; i++) {
		if(gHeightmap[index] > colorThresholds[i])
			return colors[i];
	}

	return Color(1.0f, 1.0f, 1.0f, 1.0f);
}

Color SampleColor(uint32 x, uint32 y) {
	static float32 colorThresholds[] {
//		0.7,
		0.5f,
		0.35f,
		0.25f,
		0.15f,
		0.0f,
		-0.12f,
		-0.15f,
		-0.35f,
		-1.0f
	};
	static Color colors[] {
//		Color(0.20f, 0.20f, 0.20f, 1.0f),
//		Color(0.31f, 0.25f, 0.19f, 1.0f),
		Color(0.26f, 0.25f, 0.13f, 1.0f),
		Color(0.25f, 0.21f, 0.13f, 1.0f),
		Color(0.50f, 0.33f, 0.17f, 1.0f),
		Color(0.60f, 0.43f, 0.27f, 1.0f),
		Color(0.72f, 0.535f, 0.37f, 1.0f),
		Color(0.69f, 0.54f, 0.39f, 1.0f),
		Color(0.38f, 0.54f, 0.67f, 1.0f),
		Color(0.24f, 0.30f, 0.51f, 1.0f),
		Color(0.24f, 0.30f, 0.51f, 1.0f)
	};

	uint32 elementSize = 9;

	uint32 index = (y * MAP_WIDTH) + x;
	for(uint32 i = 0; i < elementSize; i++) {
		if(gHeightmap[index] > colorThresholds[i]) {
			if(i == 0) return colors[i];
			Color fromColor = colors[i];
			Color toColor = colors[i - 1];
			Color colorDiff = toColor - fromColor;
			float32 fromThresh = colorThresholds[i];
			float32 toThresh = colorThresholds[i - 1];
			float32 threshDiff = (toThresh - fromThresh);
			threshDiff = (threshDiff >= 0) ? threshDiff : -threshDiff;
			float32 threshCompletion = (gHeightmap[index] - colorThresholds[i]) / threshDiff;
			Color addColor = (colorDiff * threshCompletion);
			Color result = fromColor + addColor;
			return result;
		}

	}

	return Color(1.0f, 1.0f, 1.0f, 1.0f);
}

uint32 XYToIndex(uint32 x, uint32 y) {
	return (y * MAP_WIDTH) + x;
}


void MainLoop () {
	app.BeginFrame();
	float64 deltaTime = app.GetDeltaTime();
	app.PollEvents();
	static float x = -0.5f;
	static float dt = 0.0f;
	float xpos =  x * sin(dt);
	dt += 0.1f;

	const float32 ZOOM_SPEED = 0.5f;
	static float32 cameraZoom = 1.0f;
	if(app.IsButtonDown(MOUSE_WHEEL_UP)) {
		cameraZoom -= ZOOM_SPEED;
	} else if (app.IsButtonDown(MOUSE_WHEEL_DOWN)) {
		cameraZoom += ZOOM_SPEED;
	}

	const float32 PAN_SPEED = 0.05f;
	static float64 cursorX = app.GetCursorX();
	static float64 cursorY = app.GetCursorY();
	float64 dx = app.GetCursorX() - cursorX;
	float64 dy = app.GetCursorY() - cursorY;
	cursorX = app.GetCursorX();
	cursorY = app.GetCursorY();
	if(app.IsButtonDown(MOUSE_BUTTON_RIGHT)) {
		gCameraTransform.position.x -= dx * PAN_SPEED;
		gCameraTransform.position.y += dy * PAN_SPEED;
		Matrix4 projection = TransformToOrtho(gCameraTransform, cameraZoom);
		DEBUGSetProjectionMatrix(&gRenderContext, projection);
	}

	const float64 MOVEMENT_SPEED = 0.1f;
	const float64 SPRINT_MULTIPLIER = 3.0f;
	Vector2 deltaPos;
	if(app.IsKeyDown(KEY_W)) deltaPos.y += 1.0f;
	if(app.IsKeyDown(KEY_S)) deltaPos.y -= 1.0f;
	if(app.IsKeyDown(KEY_D)) deltaPos.x += 1.0f;
	if(app.IsKeyDown(KEY_A)) deltaPos.x -= 1.0f;
	deltaPos.Normalize();
	deltaPos *= MOVEMENT_SPEED;
	if(app.IsKeyDown(KEY_LSHIFT)) deltaPos *= SPRINT_MULTIPLIER;
	gPlayerTransform.position += deltaPos;
	gCameraTransform.position = gPlayerTransform.position + (gPlayerTransform.size * 0.5f);
	Matrix4 projection = TransformToOrtho(gCameraTransform, cameraZoom);
	DEBUGSetProjectionMatrix(&gRenderContext, projection);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)app.GetWidth(), (float)app.GetHeight());
	io.DeltaTime = app.GetDeltaTime() > 0.0 ? app.GetDeltaTime() : 1.0f/60.0f;
	io.MousePos = ImVec2((float)app.GetCursorX(), (float)app.GetCursorY());
	io.MouseWheel = app.GetMouseWheel();
	io.MouseDown[0] = app.IsButtonDown(MOUSE_BUTTON_LEFT);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	uint32 startX, endX;
	uint32 startY, endY;
	float32 halfWidth = (gCameraTransform.size.x * 0.5f) * cameraZoom;
	float32 halfHeight = (gCameraTransform.size.y * 0.5f) * cameraZoom;
	auto left = gCameraTransform.position.x - halfWidth;
	auto right = gCameraTransform.position.x + halfWidth;
	auto bottom = gCameraTransform.position.y - halfHeight;
	auto top = gCameraTransform.position.y + halfHeight;

	startX = left > 0 ? left : 0;
	endX = right < MAP_WIDTH ? right : MAP_WIDTH - 1;
	startY = bottom > 0 ? bottom : 0;
	endY = top < MAP_HEIGHT ? top : MAP_HEIGHT - 1;


	static float32 waveAngle = 0.0f;
	static float32 waveAmp = 0.1f;
	waveAngle += deltaTime * 1.0f;
	if(waveAngle > RADIANS(360)) waveAngle = 0;
	glUseProgram(gRenderContext.shaderProgramID);
	glUniform1f(gRenderContext.waveAngleUniformLocation, waveAngle);

	//Store height information at the corners!
	//Then everything will just work...
	Vert verts[4];
	std::vector<Vector2> waterTiles;
	for(uint32 x = startX; x <= endX; x++) {
		for(uint32 y = startY; y <= endY; y++) {
			if(gHeightmap[XYToIndex(x, y)] < -0.15f) {
				waterTiles.push_back(Vector2(x, y));
			}
			uint32 index0 = XYToIndex(x, y);
			uint32 index1 = XYToIndex(x + 1, y);
			uint32 index2 = XYToIndex(x + 1, y + 1);
			uint32 index3 = XYToIndex(x, y + 1);
			verts[0] = Vert { Vector2(x, y), Vector2(0.0f, 0.0f), gTilemap[index0]};
			verts[1] = Vert { Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), gTilemap[index1] };
			verts[2] = Vert { Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), gTilemap[index2] };
			verts[3] = Vert { Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), gTilemap[index3] };
			DEBUGPushVertices(&gRenderContext, verts, 4);

//			uint32 index = (y * MAP_WIDTH) + x;
//			DEBUGFillRect(x, y, 1.0f, 1.0f, gTilemap[index], &gRenderContext);
		}
	}

	DEBUGFlushContext(&gRenderContext);


	for(Vector2& tile : waterTiles)
		DEBUGDrawTexture(textureID, tile.x, tile.y, 1.0f, 1.0f, Color(), &gRenderContext);
	glUniform1i(gRenderContext.isWaterUniformLocation, 1);
	DEBUGFlushContext(&gRenderContext);
	glUniform1i(gRenderContext.isWaterUniformLocation, 0);

	DEBUGDrawTexture(textureID, (xpos * 5.0f) + 3.0f, 7.0f, 1.0f, 1.0f, Color(), &gRenderContext);
	DEBUGFillRect(gPlayerTransform.position.x, gPlayerTransform.position.y, gPlayerTransform.size.x, gPlayerTransform.size.y, Color(0.0f, 1.0f, 0.0f, 1.0f), &gRenderContext);
	DEBUGFlushContext(&gRenderContext);

	static bool drawGrid = false;
	if(drawGrid) {
		for(uint32 x = startX; x <= endX; x++) {
			for(uint32 y = startY; y <= endY; y++) {
				DEBUGFillRect(x, y, 1.0f, 1.0f, Color(), &gRenderContext);
				}
			}
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		DEBUGFlushContext(&gRenderContext);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}



	std::stringstream stream;
	ImGui::NewFrame();
	stream << "Player Position: " << gPlayerTransform.position;
	ImGui::Text(stream.str().c_str());
	stream.str(std::string());
	stream << "Tiles Drawn: " << (endX - startX) * (endY - startY);
	ImGui::Text(stream.str().c_str());
	stream.str(std::string());
	stream << "DeltaTime: " << app.GetDeltaTime() << "  FPS: " << 1000.0f / app.GetDeltaTime();
	ImGui::Text(stream.str().c_str());

	ImGui::Checkbox("Draw grid", &drawGrid);
	//ImGui::ShowTestWindow();
	//ImGui::ShowStyleEditor();
	ImGui::Render();

	app.EndFrame();
}

int main () {
	app.Create("Raptor ImGUI", 1280, 720, false);
	ImGuiContextInit(&gImGuiContext);

	DEBUGInitRenderer(&gRenderContext, 4096);
	textureID = DEBUGLoadTexture("Assets/water.png");

	const float32 WIDTH_IN_METERS = 30.0f;
	float32 heightInMeters = WIDTH_IN_METERS * (app.GetHeight() / app.GetWidth());
	gCameraTransform.size = Vector2(WIDTH_IN_METERS, heightInMeters);
	Matrix4 projection = TransformToOrtho(gCameraTransform, 1.0f);
	DEBUGSetProjectionMatrix(&gRenderContext, projection);
	gPlayerTransform.size.x = 1.0f;
	gPlayerTransform.size.y = 1.0f;

	OpenSimplexNoise noise(0);
	gHeightmap = new float32[MAP_WIDTH_PLUS_ONE * MAP_HEIGHT_PLUS_ONE];
	gTilemap = new Color[MAP_WIDTH_PLUS_ONE * MAP_HEIGHT_PLUS_ONE];

	for(uint32 i = 0; i < MAP_WIDTH_PLUS_ONE * MAP_HEIGHT_PLUS_ONE; i++) {
		uint32 y = (i / MAP_WIDTH);
		uint32 x = i - (y * MAP_WIDTH);
		gHeightmap[i] = noise.FBM(x, y, 6, 0.01f, 0.5f);
		gTilemap[i] = SampleColor(x, y);
	}

#ifndef __EMSCRIPTEN__
	LOG_INFO("The main loop is running...");
	while (app.isRunning) {
		MainLoop();
	}

#else
	emscripten_set_main_loop(MainLoop, 0, 1);
#endif

}
