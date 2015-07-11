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

#include <SDL/SDL_mixer.h>
#include <Math/Random.hpp>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

static size_t memUsed;
static size_t memAllocated;
static size_t memFreed;

struct BiomeEntry {
	F32 startHeight;
	F32 fadeBeginHeight;
	F32 fadeEndHeight;
	GLuint textureID;
	Color color;
};

struct Biome {
	U32 entryCount;
	BiomeEntry* entries;
};

struct Terrain2D {
	uint32 width, height;
	Biome biome;
};

struct Tile {
	U32 biomeEntryID;
	F32 alpha0;
	F32 alpha1;
	F32 alpha2;
	F32 alpha3;
	bool overlaps;	//apparently this is completly evil... should totaly move it out once i do some things followed by some stuff....
	//Probably should use some type of drawlist thingy magihhuarrr nuggg
};


Terrain2D gTerrain;

Application app;

ImGuiContext gImGuiContext;
DEBUGRenderGroup gRenderGroup;
DEBUGRenderGroup terrainRenderGroup;
DEBUGRenderGroup guiRenderGroup;

Transform2D gCameraTransform;
Transform2D gPlayerTransform;

F32* gHeightmap;
F32* gAlphamap;

GLuint vertexMemorySize;
GLuint waterTextureID;
GLuint sandTextureID;
GLSLProgram* shader;

Tile* gTilemap;

Mix_Chunk* soundEffects[8];
Mix_Music* music;

ECSManager* ecs;
PhysicsSystem2D* physics2D;

bool gSoundEnabled = false;
bool gMusicEnabled = false;

void Deserialize(const char* filename) {
	using namespace rapidjson;
	std::ifstream stream;
	std::string json;
	stream.open(filename);
	if (stream.is_open()) {
		while (!stream.eof()) {
			json.push_back(stream.get());
		}
	}

	auto entryCount = 0;
	Document document;
	document.Parse(json.c_str());
	auto& biomeObj = document["biome"];
	if (biomeObj.IsArray()) {
		for (auto i = 0; i < biomeObj.Size(); i++) {
			auto& entry = biomeObj[i];
			
		}
	}
	else {
		LOG_ERROR("Could not parse biome file correctly!");
	}

}

struct Terrain2DShaderProgram {
	GLuint shaderProgramID;
	GLuint projectionUniformLocation;
	GLuint isTextureUniformLocation;
	GLuint isWaterUniformLocation;
	GLuint waveAngleUniformLocation;

	void Create() {
		shaderProgramID = DEBUGLoadShaderFromFile(ASSET(shaders/Terrain2DVertex.glsl), ASSET(shaders/Terrain2DFragment.glsl));
		projectionUniformLocation = glGetUniformLocation(shaderProgramID, "projection");
		isTextureUniformLocation = glGetUniformLocation(shaderProgramID, "isTexture");
		isWaterUniformLocation = glGetUniformLocation(shaderProgramID, "isWater");
		waveAngleUniformLocation = glGetUniformLocation(shaderProgramID, "waveAngle");
	}

	void Destroy() {
		glDeleteProgram(shaderProgramID);
	}
};

Terrain2DShaderProgram terrainShader;

static Mix_Chunk* LoadSound(const char* filename) {
	auto chunk = Mix_LoadWAV(filename);
	if (chunk == nullptr) {
		LOG_ERROR("Failed to load sound: " << filename << ": " << Mix_GetError());
		return nullptr;
	}
	return chunk;
}

static void PlaySound(Mix_Chunk* sound, uint32 loopCount = 0) {
	if (!gSoundEnabled) return;
	auto channel = Mix_PlayChannel(-1, sound, loopCount);
	if (channel == -1) {
		LOG_ERROR("AUDIO: Could not play sound! " << Mix_GetError());
	}
}

static Mix_Music* LoadMusic(const char* filename) {
	auto music = Mix_LoadMUS(filename);
	if (!music) {
		LOG_ERROR("Failed to load music: " << filename << " -- " << Mix_GetError());
	}
	return music;
}

static void PlayMusic(Mix_Music* music, uint32 loops = -1) {
	if (!gMusicEnabled) return;
	if (Mix_PlayMusic(music, loops) == -1) {
		LOG_ERROR("Couldnt not play music!" << Mix_GetError());
	}
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

class Console {
public:
	void Draw();

private:
	char inputBuffer[256];
	char historyBuffer[1024][256];
};

void Console::Draw() {
	ImGui::Begin("Console");
	ImGui::InputText("Input", inputBuffer, 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory);
	ImGui::End();
}

void CreateBiome(Biome* biome) {
	biome->entries = new BiomeEntry[2];
	biome->entryCount = 3;

	biome->entries[0].startHeight = 0.0f;
	biome->entries[0].fadeBeginHeight = 0.9f;
	biome->entries[0].fadeEndHeight = 1.0f;
	biome->entries[0].color = Color(0.69f, 0.54f, 0.39f, 1.0f),
	biome->entries[0].textureID = sandTextureID;

	biome->entries[1].startHeight = -1.0f;
	biome->entries[1].fadeBeginHeight = -0.05f;
	biome->entries[1].fadeEndHeight = 0.0f;
	biome->entries[1].color = Color(0.38f, 0.54f, 0.67f, 1.0f);
	biome->entries[1].textureID = waterTextureID;
}



uint32 XYToIndex(Terrain2D* terrain, uint32 x, uint32 y) {
	return (y * terrain->width) + x;
}

F32 SampleAlpha(Biome* biome, U32 biomeEntryID, F32 height) {
	BiomeEntry& entry = biome->entries[biomeEntryID - 1];
	if (height < entry.startHeight) {
		return 0.0f;
	}

	//if (height > entry.fadeEndHeight)
	//	return 0.0f;
	if (height < entry.fadeBeginHeight)
		return 1.0f;

	auto heightDiff = (entry.fadeEndHeight - entry.fadeBeginHeight);
	auto fadeHeight = (height - entry.fadeBeginHeight);
	auto result = 1 - (fadeHeight / heightDiff);
	return result;
}

U32 GetBiomeEntryID(Biome* biome, F32 height) {
	for (U32 i = 0; i < biome->entryCount; i++) {
		if (height > biome->entries[i].startHeight) {
			return i + 1;
		}
	}
	return 0;
}


void CreateMap(Terrain2D* terrain, U32 width, U32 height) {
	terrain->width = width;
	terrain->height = height;
	CreateBiome(&terrain->biome);

	OpenSimplexNoise noise(17);
	gHeightmap = (F32*)malloc(sizeof(F32) * (width + 1) * (height + 1));
	//gHeightmap = new float32[(width + 1) * (height + 1)];
	gTilemap = new Tile[(width + 1) * (height + 1)];
	gAlphamap = new float32[(width + 1) * (height + 1)];

	for (U32 i = 0; i < (width + 1) * (height + 1); i++) {
		U32 y = (i / width);
		U32 x = i - (y * height);
		gHeightmap[i] = noise.FBM(x, y, 6, 0.01f, 0.5f);
	}

	for (U32 x = 0; x < width; x++) {
		for (U32 y = 0; y < height; y++) {
			U32 index0 = XYToIndex(terrain, x, y);
			U32 index1 = XYToIndex(terrain, x + 1, y);
			U32 index2 = XYToIndex(terrain, x + 1, y + 1);
			U32 index3 = XYToIndex(terrain, x, y + 1);
			
			//The min height is proably all we need
			F32 minHeight = gHeightmap[index0];
			minHeight = MathUtils::Min(gHeightmap[index1], minHeight);
			minHeight = MathUtils::Min(gHeightmap[index2], minHeight);
			minHeight = MathUtils::Min(gHeightmap[index3], minHeight);
			U32 id = GetBiomeEntryID(&terrain->biome, minHeight);

#if 0
			F32 maxHeight = gHeightmap[index0];

			maxHeight = MathUtils::Max(gHeightmap[index1], maxHeight);
			maxHeight = MathUtils::Max(gHeightmap[index2], maxHeight);
			maxHeight = MathUtils::Max(gHeightmap[index3], maxHeight);
			//I dont think its nessecary to actauly check this!
			//Asume the biome creator is not mentaly retarted!
			U32 minID = GetBiomeEntryID(&terrain->biome, minHeight);
			U32 maxID = GetBiomeEntryID(&terrain->biome, maxHeight);

			if (minID != maxID)  
				gTilemap[index0].overlaps = true;
			else 
				gTilemap[index0].overlaps = false;
			gTilemap[index0].biomeEntryID = minID;
#endif
			gTilemap[index0].biomeEntryID = id;
			auto& entry = terrain->biome.entries[id - 1];
			if (gHeightmap[index0] > entry.fadeBeginHeight ||
				gHeightmap[index1] > entry.fadeBeginHeight ||
				gHeightmap[index2] > entry.fadeBeginHeight ||
				gHeightmap[index3] > entry.fadeBeginHeight ) {
				gTilemap[index0].overlaps = true;
			}
			else {
				gTilemap[index0].overlaps = false;
			}

			gTilemap[index0].alpha0 = SampleAlpha(&terrain->biome, id, gHeightmap[index0]);
			gTilemap[index0].alpha1 = SampleAlpha(&terrain->biome, id, gHeightmap[index1]);
			gTilemap[index0].alpha2 = SampleAlpha(&terrain->biome, id, gHeightmap[index2]);
			gTilemap[index0].alpha3 = SampleAlpha(&terrain->biome, id, gHeightmap[index3]);
		}
	}
}



void MainLoop () {
	BENCHMARK_START(mainLoop);
	app.BeginFrame();
	float64 deltaTime = app.GetDeltaTime();
	app.PollEvents();
	if (app.IsKeyDown(KEY_ESCAPE)) {
		app.isRunning = false;
	}

	glUseProgram(terrainShader.shaderProgramID);

	static float x = -0.5f;
	static float dt = 0.0f;
	float xpos =  x * sin(dt);
	dt += 0.1f;

	const float32 ZOOM_SPEED = 0.5f;
	static float32 cameraZoom = 1.0f;
	cameraZoom -= app.GetMouseWheel() * ZOOM_SPEED;

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
	}

	const F64 MOVEMENT_SPEED = 0.1f;
	const F64 SPRINT_MULTIPLIER = 3.0f;
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
	glUniformMatrix4fv(terrainShader.projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);

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
	endX = right < gTerrain.width ? right : gTerrain.width - 1;
	startY = bottom > 0 ? bottom : 0;
	endY = top < gTerrain.height ? top : gTerrain.height - 1;

	static float32 waveAngle = 0.0f;
	static float32 waveAmp = 0.1f;
	waveAngle += deltaTime * 1.0f;
	if(waveAngle > RADIANS(360)) waveAngle = 0;
	glUseProgram(terrainShader.shaderProgramID);
	glUniform1f(terrainShader.waveAngleUniformLocation, waveAngle);

	static bool drawBaseColor = true;

	BENCHMARK_START(draw);
	Vert verts[4];
	static bool drawWater = true;
	static bool drawSand = true;
	for(U32 x = startX; x <= endX; x++) {
		for(U32 y = startY; y <= endY; y++) {
			auto& tile = gTilemap[XYToIndex(&gTerrain, x, y)];
			if (tile.overlaps) {
				U32 id = tile.biomeEntryID - 1;
				if (id != 0) {
					auto& baseEntry = gTerrain.biome.entries[id - 1];
					verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), baseEntry.color };
					verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), baseEntry.color };
					verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), baseEntry.color };
					verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), baseEntry.color };
					DEBUGPushVertices(&gRenderGroup, verts, 4);
					gRenderGroup.currentTextureID = baseEntry.textureID;
					DEBUGFlushGroup(&gRenderGroup);
				}
			}

			if (!drawWater && tile.biomeEntryID == 2) continue;
			if (!drawSand && tile.biomeEntryID == 1) continue;
			BiomeEntry& entry = gTerrain.biome.entries[tile.biomeEntryID - 1];
			verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha0) };
			verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha1) };
			verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha2) };
			verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha3) };
			DEBUGPushVertices(&gRenderGroup, verts, 4);
			gRenderGroup.currentTextureID = entry.textureID;
			DEBUGFlushGroup(&gRenderGroup);

		}
	}
	BENCHMARK_END(draw);


	//glUniform1i(terrainShader.isTextureUniformLocation, 1);
	//static bool drawSand = true;
	//if (drawSand) {
	//	for (Vector2& tile : sandTiles) {
	//		uint32 x = tile.x;
	//		uint32 y = tile.y;
	//		uint32 index0 = XYToIndex(&gTerrain, x, y);
	//		uint32 index1 = XYToIndex(&gTerrain, x + 1, y);
	//		uint32 index2 = XYToIndex(&gTerrain, x + 1, y + 1);
	//		uint32 index3 = XYToIndex(&gTerrain, x, y + 1);

	//		float32 from = 0.0f;
	//		float32 to = -0.15f;
	//		float32 diff = (to - from);
	//		float32 alpha0 = 1.0 - ((gHeightmap[index0] - from) / diff);
	//		float32 alpha1 = 1.0 - ((gHeightmap[index1] - from) / diff);
	//		float32 alpha2 = 1.0 - ((gHeightmap[index2] - from) / diff);
	//		float32 alpha3 = 1.0 - ((gHeightmap[index3] - from) / diff);
	//		//verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), Color(gTilemap[index0].r, gTilemap[index0].g, gTilemap[index0].b, gAlphamap[index0]) };
	//		//verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), Color(gTilemap[index1].r, gTilemap[index1].g, gTilemap[index1].b, gAlphamap[index1]) };
	//		//verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), Color(gTilemap[index2].r, gTilemap[index2].g, gTilemap[index2].b, gAlphamap[index2]) };
	//		//verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), Color(gTilemap[index3].r, gTilemap[index3].g, gTilemap[index3].b, gAlphamap[index3]) };

	//		verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), Color(0.69f, 0.54f, 0.39f, 1.0f) };
	//		verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), Color(0.69f, 0.54f, 0.39f, 1.0f) };
	//		verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), Color(0.69f, 0.54f, 0.39f, 1.0f) };
	//		verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), Color(0.69f, 0.54f, 0.39f, 1.0f) };

	//		/*
	//		verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), gTilemap[index0]};
	//		verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), gTilemap[index1] };
	//		verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), gTilemap[index2] };
	//		verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), gTilemap[index3] };
	//		*/
	//		//verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), Color(1.0f, 1.0f, 1.0f, gAlphamap[index0]) };
	//		//verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), Color(1.0f, 1.0f, 1.0f, gAlphamap[index1]) };
	//		//verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, gAlphamap[index2]) };
	//		//verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, gAlphamap[index3]) };

	//		//verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), Color(gTilemap[index0].r, gTilemap[index0].g, gTilemap[index0].b, alpha0) };
	//		//verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), Color(gTilemap[index1].r, gTilemap[index1].g, gTilemap[index1].b, alpha1) };
	//		//verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), Color(gTilemap[index2].r, gTilemap[index2].g, gTilemap[index2].b, alpha2) };
	//		//verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), Color(gTilemap[index3].r, gTilemap[index3].g, gTilemap[index3].b, alpha3) };

	//		DEBUGPushVertices(&gRenderGroup, verts, 4);
	//	}

	//	gRenderGroup.currentTextureID = sandTextureID;
	//	DEBUGFlushGroup(&gRenderGroup);
	//}


	//static bool drawWater = true;
	//if (drawWater) {
	//	for (Vector2& tile : waterTiles) {
	//		uint32 x = tile.x;
	//		uint32 y = tile.y;
	//		uint32 index0 = XYToIndex(&gTerrain, x, y);
	//		uint32 index1 = XYToIndex(&gTerrain, x + 1, y);
	//		uint32 index2 = XYToIndex(&gTerrain, x + 1, y + 1);
	//		uint32 index3 = XYToIndex(&gTerrain, x, y + 1);

	//		float32 from = -0.12f;
	//		float32 to = -0.15f;
	//		float32 diff = (to - from);
	//		float32 alpha0 = ((gHeightmap[index0] - from) / diff);
	//		float32 alpha1 = ((gHeightmap[index1] - from) / diff);
	//		float32 alpha2 = ((gHeightmap[index2] - from) / diff);
	//		float32 alpha3 = ((gHeightmap[index3] - from) / diff);
	//		//verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), Color(gTilemap[index0].r, gTilemap[index0].g, gTilemap[index0].b, gAlphamap[index0]) };
	//		//verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), Color(gTilemap[index1].r, gTilemap[index1].g, gTilemap[index1].b, gAlphamap[index1]) };
	//		//verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), Color(gTilemap[index2].r, gTilemap[index2].g, gTilemap[index2].b, gAlphamap[index2]) };
	//		//verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), Color(gTilemap[index3].r, gTilemap[index3].g, gTilemap[index3].b, gAlphamap[index3]) };

	//		//verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), gTilemap[index0]};
	//		//verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), gTilemap[index1] };
	//		//verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), gTilemap[index2] };
	//		//verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), gTilemap[index3] };

	//		//verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), Color(1.0f, 1.0f, 1.0f, gAlphamap[index0]) };
	//		//verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), Color(1.0f, 1.0f, 1.0f, gAlphamap[index1]) };
	//		//verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, gAlphamap[index2]) };
	//		//verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, gAlphamap[index3]) };

	//		

	//		verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), Color(0.38f, 0.54f, 0.67f, alpha0) };
	//		verts[1] = Vert{ Vector2(x + 1.0, y), Vector2(1.0f, 0.0f), Color(0.38f, 0.54f, 0.67f, alpha1) };
	//		verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), Vector2(1.0f, 1.0f), Color(0.38f, 0.54f, 0.67f, alpha2) };
	//		verts[3] = Vert{ Vector2(x, y + 1.0), Vector2(0.0f, 1.0f), Color(0.38f, 0.54f, 0.67f, alpha3) };

	//		DEBUGPushVertices(&gRenderGroup, verts, 4);
	//	}

	//	glUniform1i(terrainShader.isWaterUniformLocation, 1);
	//	gRenderGroup.currentTextureID = waterTextureID;
	//	DEBUGFlushGroup(&gRenderGroup);
	//	glUniform1i(terrainShader.isWaterUniformLocation, 0);
	//}


	DEBUGFillRect(&gRenderGroup, gPlayerTransform.position.x, gPlayerTransform.position.y, gPlayerTransform.size.x, gPlayerTransform.size.y, Color(0.0f, 1.0f, 0.0f, 1.0f));
	DEBUGFlushGroup(&gRenderGroup);

	static bool drawGrid = false;
	if(drawGrid) {
		for(uint32 x = startX; x <= endX; x++) {
			for(uint32 y = startY; y <= endY; y++) {
				DEBUGFillRect(&gRenderGroup, x, y, 1.0f, 1.0f, Color());
				}
			}
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		DEBUGFlushGroup(&gRenderGroup);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	//@SOUNDS
	static Random rng(0);
	static float32 waveTime = 0;
	static float32 nextWaveTime = rng.Range(1.5f, 3.0f);
	waveTime += deltaTime;
	if (waveTime >= nextWaveTime) {
		uint32 soundIndex = rng.Range(0, 7);
		PlaySound(soundEffects[soundIndex]);
		waveTime = 0;
		nextWaveTime = rng.Range(1.5f, 3.0f);
	} 

	BENCHMARK_END(mainLoop);
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
	stream.str(std::string());
	stream << "DrawCalls: " << gRenderGroup.drawCalls;
	ImGui::Text(stream.str().c_str());
	gRenderGroup.drawCalls = 0;

	ImGui::Checkbox("Draw grid", &drawGrid);
	ImGui::Checkbox("Draw base color", &drawBaseColor);
	ImGui::Checkbox("Draw water", &drawWater);
	ImGui::Checkbox("Draw sand", &drawSand);

	if (ImGui::Button("Reload Shader")) {
		terrainShader.Destroy();
		terrainShader.Create();
	}

	if (ImGui::Button("Reload textures")) {
		glDeleteTextures(1, &waterTextureID);
		glDeleteTextures(1, &sandTextureID);
		waterTextureID = DEBUGLoadTexture(ASSET(textures/water.png));
		sandTextureID = DEBUGLoadTexture(ASSET(textures/sand.png));
	}

	ImGui::Begin("Profiler");
	static const uint32 resolution = 32;
	static uint32 cntr = 0;
	static float32 frameTimes[resolution];
	frameTimes[cntr] = deltaTime * 1000.0f;
	if (++cntr == resolution) cntr = 0;
	ImGui::Text((std::string("Tile draw Time: ") + std::to_string(benchmark_draw_time)).c_str());
	ImGui::PlotLines("##MainLoop", frameTimes, resolution, cntr, "MainLoop", 0.0f, 100.0f, ImVec2(400, 75));
	ImGui::End();

	ImGui::Begin("Tile Info");
	U32 tileX = (U32)gPlayerTransform.position.x;
	U32 tileY = (U32)gPlayerTransform.position.y;
	auto& tile = gTilemap[XYToIndex(&gTerrain, tileX, tileY)];
	ImGui::Text((std::string("X:") + std::to_string(tileX) + std::string(" Y: ") + std::to_string(tileY)).c_str());
	ImGui::Text((std::string("Height: ") + std::to_string(gHeightmap[XYToIndex(&gTerrain, tileX, tileY)])).c_str());
	ImGui::Text((std::string("Overlaps: ") + std::string(tile.overlaps ? "True" : "False")).c_str());

	ImGui::Text((std::string("Alpha0: ") + std::to_string(tile.alpha0)).c_str());
	ImGui::Text((std::string("Alpha1: ") + std::to_string(tile.alpha1)).c_str());
	ImGui::Text((std::string("Alpha2: ") + std::to_string(tile.alpha2)).c_str());
	ImGui::Text((std::string("Alpha3: ") + std::to_string(tile.alpha3)).c_str());
	ImGui::End();

	//@CONSOLE
	static Console console;
	console.Draw();

	//ImGui::Begin("Example: Fixed Overlay", 0, ImVec2(0, 0), 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	//ImGui::Text("Test");
	//ImGui::End();

	//ImGui::ShowTestWindow();
	//ImGui::ShowStyleEditor();
	ImGui::Render();

	app.EndFrame();
}

int main () {
	app.Create("Raptor ImGUI", 1280, 720, true);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ImGuiContextInit(&gImGuiContext);

	DEBUGCreateRenderGroup(&gRenderGroup, 4096);
	terrainShader.Create();

	soundEffects[0] = LoadSound(ASSET(sounds/wave00.ogg));
	soundEffects[1] = LoadSound(ASSET(sounds/wave01.ogg));
	soundEffects[2] = LoadSound(ASSET(sounds/wave02.ogg));
	soundEffects[3] = LoadSound(ASSET(sounds/wave03.ogg));
	soundEffects[4] = LoadSound(ASSET(sounds/wave04.ogg));
	soundEffects[5] = LoadSound(ASSET(sounds/wave05.ogg));
	soundEffects[6] = LoadSound(ASSET(sounds/wave06.ogg));
	soundEffects[7] = LoadSound(ASSET(sounds/wave07.ogg));

	music = LoadMusic(ASSET(music/celestial.mp3));
	PlayMusic(music);


	waterTextureID = DEBUGLoadTexture(ASSET(textures/water.png));
	sandTextureID = DEBUGLoadTexture(ASSET(textures/sand.png));

	const float32 WIDTH_IN_METERS = 30.0f;
	float32 heightInMeters = WIDTH_IN_METERS * (app.GetHeight() / app.GetWidth());
	gCameraTransform.size = Vector2(WIDTH_IN_METERS, heightInMeters);
	gPlayerTransform.size.x = 1.0f;
	gPlayerTransform.size.y = 1.0f;


	CreateMap(&gTerrain, 512, 512);


#ifndef __EMSCRIPTEN__
	LOG_INFO("The main loop is running...");
	while (app.isRunning) {
		MainLoop();
	}

#else
	emscripten_set_main_loop(MainLoop, 0, 1);
#endif

	return 0;
}
