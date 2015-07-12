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

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>\

#define TILE_SIZE_METERS 2

struct BiomeEntry {
	U32 regionIndex;
	F32 startHeight;
	F32 fadeBeginHeight;
	F32 fadeEndHeight;
	Color color;
};

template<class Archive>
void serialize(Archive& archive, Vector2& vector) {
	archive(cereal::make_nvp("X", vector.x), 
			cereal::make_nvp("Y", vector.y));
}

template<class Archive>
void serialize(Archive & archive, BiomeEntry& entry) {
	archive(cereal::make_nvp("StartHeight", entry.startHeight),
			cereal::make_nvp("FadeBegin", entry.fadeBeginHeight),
			cereal::make_nvp("FadeEnd", entry.fadeEndHeight),
			cereal::make_nvp("Color" , entry.color),
			cereal::make_nvp("RegionIndex", entry.regionIndex));
}

struct Biome {
	GLuint textureID;
	U32 entryCount;
	BiomeEntry* entries;
};

template<class Archive>
void serialize(Archive& archive, Color& color) {
	archive(
			cereal::make_nvp("r", color.r),
			cereal::make_nvp("g", color.g),
			cereal::make_nvp("b", color.b),
			cereal::make_nvp("a", color.a)
		);
}

template<class Archive>
void save(Archive& archive, const Biome& biome) {
	archive(cereal::make_size_tag(biome.entryCount));
	for (auto i = 0; i < biome.entryCount; i++) {
		archive(biome.entries[i]);
	}
}

template<class Archive>
void load(Archive& archive, Biome& biome) {
	cereal::size_type size;
	archive(cereal::make_size_tag(size));
	biome.entryCount = (U32)size;
	biome.entries = (BiomeEntry*)malloc(sizeof(BiomeEntry) * size);
	for (auto i = 0; i < biome.entryCount; i++) {
		archive(biome.entries[i]);
	}
}

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

GLuint spriteProgramID;
Terrain2D gTerrain;

Application app;

ImGuiContext gImGuiContext;
DEBUGRenderGroup gRenderGroup;

Transform2D gCameraTransform;
Transform2D gPlayerTransform;

F32* gHeightmap;
F32* gAlphamap;

GLuint vertexMemorySize;
GLuint waterTextureID;
GLuint sandTextureID;
GLuint mapTextureID;
GLuint nullTextureID;
GLSLProgram* shader;

Tile* gTilemap;

Mix_Chunk* soundEffects[8];
Mix_Music* music;

ECSManager* ecs;
PhysicsSystem2D* physics2D;

bool gSoundEnabled = false;
bool gMusicEnabled = false;

struct GPUProgram {
	GLuint id;
};

struct TextureRegion {
	Vector2 uvs[4];
};	
TextureRegion* regions;

void UseProgram(GPUProgram* program) {
	glUseProgram(program->id);
}

struct Terrain2DShaderProgram {
	GLuint shaderProgramID;
	GLuint projectionUniformLocation;
	GLuint isTextureUniformLocation;
	GLuint isWaterUniformLocation;
	GLuint waveAngleUniformLocation;

	void Create() {
		shaderProgramID = DEBUGLoadShaderFromFile(ASSET("shaders/Terrain2DVertex.glsl"), ASSET("shaders/Terrain2DFragment.glsl"));
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

GLuint CreateTextureFromPixels(U32 width, U32 height, U8* pixels) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return textureID;
}

GLuint TerrainToTexture(Terrain2D* terrain) {
	U8* pixels = new U8[terrain->width * terrain->height * 4];
	for (U32 i = 0; i < terrain->width * terrain->height * 4; i+=4) {
		U32 index = i / 4;
		auto& entry = terrain->biome.entries[gTilemap[index].biomeEntryID - 1];
		pixels[i + 0] = entry.color.r * 255;
		pixels[i + 1] = entry.color.g * 255;
		pixels[i + 2] = entry.color.b * 255;
		pixels[i + 3] = entry.color.a * 255;

	}
	auto result = CreateTextureFromPixels(terrain->width, terrain->height, pixels);
	delete[] pixels;
	return result;
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
	biome->entries[0].color = Color(0.69f, 0.54f, 0.39f, 1.0f);

	biome->entries[1].startHeight = -1.0f;
	biome->entries[1].fadeBeginHeight = -0.05f;
	biome->entries[1].fadeEndHeight = 0.0f;
	biome->entries[1].color = Color(0.38f, 0.54f, 0.67f, 1.0f);

	{
		std::ofstream os(ASSET("test.json"));
		cereal::JSONOutputArchive archive(os);
		
		archive(cereal::make_nvp("Texture", "Textureid..."));
		archive(cereal::make_nvp("Entries", *biome));
	}
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

Vector2 GetTexelCoords(U32 x, U32 y, U32 width, U32 height) {
	F32 u = (F32)(x) / (F32)width;
	F32 v = (F32)(y) / (F32)height;
	return Vector2(u, v);
}


void ReloadBiome() {
	auto terrain = &gTerrain;

	const U32 textureWidth = 128;
	const U32 textureHeight = 128;
	regions[0].uvs[0] = GetTexelCoords(0, 0, textureWidth, textureHeight);
	regions[0].uvs[1] = GetTexelCoords(64, 0, textureWidth, textureHeight);
	regions[0].uvs[2] = GetTexelCoords(64, 64, textureWidth, textureHeight);
	regions[0].uvs[3] = GetTexelCoords(0, 64, textureWidth, textureHeight);

	regions[1].uvs[0] = GetTexelCoords(64, 0, textureWidth, textureHeight);
	regions[1].uvs[1] = GetTexelCoords(128, 0, textureWidth, textureHeight);
	regions[1].uvs[2] = GetTexelCoords(128, 64, textureWidth, textureHeight);
	regions[1].uvs[3] = GetTexelCoords(64, 64, textureWidth, textureHeight);

	regions[2].uvs[0] = GetTexelCoords(0, 64, textureWidth, textureHeight);
	regions[2].uvs[1] = GetTexelCoords(64, 64, textureWidth, textureHeight);
	regions[2].uvs[2] = GetTexelCoords(64, 128, textureWidth, textureHeight);
	regions[2].uvs[3] = GetTexelCoords(0, 128, textureWidth, textureHeight);

	regions[3].uvs[0] = GetTexelCoords(64, 64, textureWidth, textureHeight);
	regions[3].uvs[1] = GetTexelCoords(128, 64, textureWidth, textureHeight);
	regions[3].uvs[2] = GetTexelCoords(128, 128, textureWidth, textureHeight);
	regions[3].uvs[3] = GetTexelCoords(64, 128, textureWidth, textureHeight);

	std::string textureFilename;
	{
		std::ifstream is(ASSET("biome.json"));
		cereal::JSONInputArchive archive(is);
		archive(cereal::make_nvp("Texture", textureFilename));
		archive(cereal::make_nvp("Entries", terrain->biome));
	}
	//terrain->biome.textureID = DEBUGLoadTexture(std::string(ASSET_DIR) + textureFilename);
	terrain->biome.textureID = DEBUGLoadTexture(ASSET("textures/biome.png"));

	for (U32 x = 0; x < terrain->width; x++) {
		for (U32 y = 0; y < terrain->height; y++) {
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

			gTilemap[index0].biomeEntryID = id;
			auto& entry = terrain->biome.entries[id - 1];
			if (gHeightmap[index0] > entry.fadeBeginHeight ||
				gHeightmap[index1] > entry.fadeBeginHeight ||
				gHeightmap[index2] > entry.fadeBeginHeight ||
				gHeightmap[index3] > entry.fadeBeginHeight) {
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

	glDeleteTextures(1, &mapTextureID);
	mapTextureID = TerrainToTexture(terrain);
}

void ReloadHeightmap() {
	auto terrain = &gTerrain;
	OpenSimplexNoise noise(time(NULL));
	for (U32 i = 0; i < (terrain->width + 1) * (terrain->height + 1); i++) {
		U32 y = (i / terrain->width);
		U32 x = i - (y * terrain->height);
		gHeightmap[i] = noise.FBM(x, y, 6, 0.006f, 0.5f);
	}

	OpenSimplexNoise otherNoise(time(NULL));
	for (U32 i = 0; i < (terrain->width + 1) * (terrain->height + 1); i++) {
		U32 y = (i / terrain->width);
		U32 x = i - (y * terrain->height);
		gHeightmap[i] += otherNoise.FBM(x, y, 6, 0.0006f, 0.5f) * 0.45f;
		gHeightmap[i] = MathUtils::Clamp(gHeightmap[i], -1.0f, 1.0f);
	}

	OpenSimplexNoise low(time(NULL));
	for (U32 i = 0; i < (terrain->width + 1) * (terrain->height + 1); i++) {
		U32 y = (i / terrain->width);
		U32 x = i - (y * terrain->height);
		gHeightmap[i] -= low.RidgedNoise(x, y, 6, 0.06f, 0.5f) * 0.025f;
		gHeightmap[i] = MathUtils::Clamp(gHeightmap[i], -1.0f, 1.0f);
	}



	ReloadBiome();
}

void CreateMap(Terrain2D* terrain, U32 width, U32 height) {
	terrain->width = width;
	terrain->height = height;

	//CreateBiome(&terrain->biome);

	gHeightmap = (F32*)malloc(sizeof(F32) * (width + 1) * (height + 1));
	//gHeightmap = new float32[(width + 1) * (height + 1)];
	gTilemap = new Tile[(width + 1) * (height + 1)];
	gAlphamap = new float32[(width + 1) * (height + 1)];
	regions = new TextureRegion[4];
	
	ReloadHeightmap();
	ReloadBiome();
}





void MainLoop () {
	BENCHMARK_START(mainLoop);
	app.BeginFrame();
	float64 deltaTime = app.GetDeltaTime();
	app.PollEvents();
	if (app.IsKeyDown(KEY_ESCAPE)) {
		app.isRunning = false;
	}

	static float x = -0.5f;
	static float dt = 0.0f;
	float xpos =  x * sin(dt);
	dt += 0.1f;

	const float32 ZOOM_SPEED = 0.5f;
	static float32 cameraZoom = 1.0f;
	float32 zoomSpeed = (cameraZoom) * ZOOM_SPEED; 
	cameraZoom -= app.GetMouseWheel() * zoomSpeed;

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


	BENCHMARK_START(draw);
	static bool drawMapTexture = true;
	static bool drawWater = true;
	static bool drawSand = true;
	if (drawMapTexture) {
		glUseProgram(spriteProgramID);
		Matrix4 projection = TransformToOrtho(gCameraTransform, cameraZoom);
		glUniformMatrix4fv(glGetUniformLocation(spriteProgramID, "projection"), 1, GL_FALSE, &projection[0][0]);
		DEBUGDrawTexture(&gRenderGroup, mapTextureID, 0, 0, 512, 512, Color());
		DEBUGFlushGroup(&gRenderGroup);
	} else {
		glUseProgram(terrainShader.shaderProgramID);
		glUniform1i(terrainShader.isWaterUniformLocation, 1);
		Matrix4 projection = TransformToOrtho(gCameraTransform, cameraZoom);
		glUniformMatrix4fv(terrainShader.projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);
		gRenderGroup.currentTextureID = gTerrain.biome.textureID;


		std::vector<U32> waterTiles;
		Vert verts[4];
		for (U32 x = startX; x <= endX; x++) {
			for (U32 y = startY; y <= endY; y++) {
				auto& tile = gTilemap[XYToIndex(&gTerrain, x, y)];
				if (tile.biomeEntryID == 1 || tile.biomeEntryID == 2) {
					waterTiles.push_back(XYToIndex(&gTerrain, x, y));
					continue;
				}

				if (tile.overlaps) {
					U32 id = tile.biomeEntryID - 1;
					if (id != 0) {
						auto& baseEntry = gTerrain.biome.entries[id - 1];
						verts[0] = Vert{ Vector2(x, y), regions[baseEntry.regionIndex].uvs[0], baseEntry.color };
						verts[1] = Vert{ Vector2(x + 1.0, y), regions[baseEntry.regionIndex].uvs[1], baseEntry.color };
						verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), regions[baseEntry.regionIndex].uvs[2], baseEntry.color };
						verts[3] = Vert{ Vector2(x, y + 1.0), regions[baseEntry.regionIndex].uvs[3], baseEntry.color };
						DEBUGPushVertices(&gRenderGroup, verts, 4);

					}
				}


				if (!drawWater && tile.biomeEntryID == 2) continue;
				if (!drawSand && tile.biomeEntryID == 1) continue;
				BiomeEntry& entry = gTerrain.biome.entries[tile.biomeEntryID - 1];
				verts[0] = Vert{ Vector2(x, y), regions[entry.regionIndex].uvs[0], Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha0) };
				verts[1] = Vert{ Vector2(x + 1.0, y), regions[entry.regionIndex].uvs[1], Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha1) };
				verts[2] = Vert{ Vector2(x + 1.0, y + 1.0), regions[entry.regionIndex].uvs[2], Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha2) };
				verts[3] = Vert{ Vector2(x, y + 1.0), regions[entry.regionIndex].uvs[3], Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha3) };
				DEBUGPushVertices(&gRenderGroup, verts, 4);
				{

					DEBUGFlushGroup(&gRenderGroup);
					//glUniform1i(terrainShader.isWaterUniformLocation, 0);
				} 
				
			}
		}
	}
	DEBUGFlushGroup(&gRenderGroup);
	BENCHMARK_END(draw);

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
	stream << "Camera Zoom: " << cameraZoom;
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
	ImGui::Checkbox("Draw base color", &drawMapTexture);
	ImGui::Checkbox("Draw water", &drawWater);
	ImGui::Checkbox("Draw sand", &drawSand);

	//@BUTTONS
	if (ImGui::Button("Reload Shader")) {
		terrainShader.Destroy();
		terrainShader.Create();
	}
	if (ImGui::Button("Reload textures")) {
		glDeleteTextures(1, &waterTextureID);
		glDeleteTextures(1, &sandTextureID);
		waterTextureID = DEBUGLoadTexture(ASSET("textures/water.png"));
		sandTextureID = DEBUGLoadTexture(ASSET("textures/sand.png"));
	}

	if (ImGui::Button("Reload Biome")) ReloadBiome();
	if (ImGui::Button("Reload Heightmap")) ReloadHeightmap();



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
	spriteProgramID = DEBUGLoadShaderFromFile(ASSET("shaders/Sprite.vert"), ASSET("shaders/Sprite.frag"));

	soundEffects[0] = LoadSound(ASSET("sounds/wave00.ogg"));
	soundEffects[1] = LoadSound(ASSET("sounds/wave01.ogg"));
	soundEffects[2] = LoadSound(ASSET("sounds/wave02.ogg"));
	soundEffects[3] = LoadSound(ASSET("sounds/wave03.ogg"));
	soundEffects[4] = LoadSound(ASSET("sounds/wave04.ogg"));
	soundEffects[5] = LoadSound(ASSET("sounds/wave05.ogg"));
	soundEffects[6] = LoadSound(ASSET("sounds/wave06.ogg"));
	soundEffects[7] = LoadSound(ASSET("sounds/wave07.ogg"));

	music = LoadMusic(ASSET("music/celestial.mp3"));
	PlayMusic(music);


	nullTextureID = DEBUGLoadTexture(ASSET("textures/null.png"));
	waterTextureID = DEBUGLoadTexture(ASSET("textures/water.png"));
	sandTextureID = DEBUGLoadTexture(ASSET("textures/sand.png"));

	const float32 WIDTH_IN_METERS = 30.0f;
	float32 heightInMeters = WIDTH_IN_METERS * (app.GetHeight() / app.GetWidth());
	gCameraTransform.size = Vector2(WIDTH_IN_METERS, heightInMeters);
	gPlayerTransform.size.x = 1.0f;
	gPlayerTransform.size.y = 1.0f;



	CreateMap(&gTerrain, 512, 512);
	mapTextureID = TerrainToTexture(&gTerrain);



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
