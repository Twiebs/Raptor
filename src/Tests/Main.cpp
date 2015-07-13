#include <string>
#include <sstream>
#include <malloc.h>

#include <Application/Application.hpp>
#include <GL/glew.h>
#include <Utils/IO.hpp>

#include <Core/Common.hpp>
#include <Core/Network.hpp>
#include <Core/InputListener.hpp>

#include <ECS/ECSManager.hpp>
#include <Base/Transform2D.hpp>
#include <Base/RenderSystem2D.hpp>
#include <Base/PhysicsSystem2D.hpp>

#include <Graphics/Color.hpp>
#include <Graphics/Texture.hpp>

#include <Math/MathUtils.hpp>
#include <Math/Matrix4.hpp>
#include <Math/Noise.hpp>

#include <imgui.h>
#include <Platform/ImGui.hpp>

#include <SDL2/SDL_mixer.h>
#include <Math/Random.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>

#define TILE_SIZE_METERS 3
#define VIEWPORT_WIDTH_IN_METERS 30

struct TileVertex {
	Vector2 position;
	Vector3 texCoord;
	Color color;
};

struct BiomeEntry {
	std::string texture;
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
	archive(cereal::make_nvp("Texture", entry.texture),
			cereal::make_nvp("StartHeight", entry.startHeight),
			cereal::make_nvp("FadeBegin", entry.fadeBeginHeight),
			cereal::make_nvp("FadeEnd", entry.fadeEndHeight),
			cereal::make_nvp("Color" , entry.color)
			);
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
	//biome.entries = (BiomeEntry*)malloc(sizeof(BiomeEntry) * size);
	biome.entries = new BiomeEntry[size];
	for (auto i = 0; i < biome.entryCount; i++) {
		archive(biome.entries[i]);
	}
}

struct Tile {
	U32 biomeEntryID;
	F32 alpha0;
	F32 alpha1;
	F32 alpha2;
	F32 alpha3;
	bool overlaps;	//apparently this is completly evil... should totaly move it out once i do some things followed by some stuff....
	//Probably should use some type of drawlist thingy magihhuarrr nuggg
};

//woof woof!
struct Terrain2D {
	U32 widthInTiles, heightInTiles;
	U8* memblock;
	F32* heightmap;
	Tile* tilemap;
	Biome biome;
};



GLuint spriteProgramID;
Terrain2D gTerrain;

Application app;

ImGuiContext gImGuiContext;
DEBUGRenderGroup gRenderGroup;
DEBUGBufferGroup gRenderBuffer;

Transform2D gCameraTransform;
Transform2D gPlayerTransform;

GLuint mapTextureID;
GLuint nullTextureID;

Mix_Chunk* soundEffects[8];
Mix_Music* music;

struct Kactus {
	Vector2 position;
	GLuint textureID;
};

#define MAX_ENTITES 100000
U32 gEntityCount;
Vector2 entityPositions[MAX_ENTITES];
GLuint entityTexureID[MAX_ENTITES];
Kactus entities[MAX_ENTITES];

#define FOLIAGE_COUNT 47
GLuint foliageTextures[FOLIAGE_COUNT];

bool gSoundEnabled = false;
bool gMusicEnabled = false;

struct GPUProgram {
	GLuint id;
};

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
	F32 halfWidth = transform.size.x * 0.5f * zoom;
	F32 halfHeight = transform.size.y * 0.5f * zoom;
	return Matrix4::Ortho(
			(transform.position.x - halfWidth),
			(transform.position.x + halfWidth),
			(transform.position.y - halfHeight),
			(transform.position.y + halfHeight),
			1.0f
		);
}

GLuint TerrainToTexture(Terrain2D* terrain) {
	U8* pixels = new U8[terrain->widthInTiles * terrain->heightInTiles * 4];
	for (U32 i = 0; i < terrain->widthInTiles * terrain->heightInTiles * 4; i+=4) {
		U32 index = i / 4;
		auto& entry = terrain->biome.entries[terrain->tilemap[index].biomeEntryID - 1];
		pixels[i + 0] = entry.color.r * 255;
		pixels[i + 1] = entry.color.g * 255;
		pixels[i + 2] = entry.color.b * 255;
		pixels[i + 3] = entry.color.a * 255;

	}
	auto result = CreateTextureFromPixels(terrain->widthInTiles, terrain->heightInTiles, pixels);
	delete[] pixels;
	return result;
}

void CreateBiome(Biome* biome) {
	biome->entries = new BiomeEntry[1];
	biome->entryCount = 1;

	biome->entries[0].texture = "mytexture.png";
	biome->entries[0].startHeight = 0.0f;
	biome->entries[0].fadeBeginHeight = 0.9f;
	biome->entries[0].fadeEndHeight = 1.0f;
	biome->entries[0].color = Color(0.69f, 0.54f, 0.39f, 1.0f);

	{
		std::ofstream os(ASSET("EXAMPLE.json"));
		cereal::JSONOutputArchive archive(os);
		archive(cereal::make_nvp("Entries", *biome));
	}
}



uint32 XYToIndex(Terrain2D* terrain, uint32 x, uint32 y) {
	return (y * terrain->widthInTiles) + x;
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



void LoadBiome(Terrain2D* terrain) {
	{
		std::ifstream is(ASSET("biome.json"));
		cereal::JSONInputArchive archive(is);
		archive(cereal::make_nvp("Entries", terrain->biome));
	}

	std::vector<std::string> entryTextureFiles;
	for (auto i = 0; i < terrain->biome.entryCount; i++)
		entryTextureFiles.push_back(terrain->biome.entries[i].texture);
	terrain->biome.textureID = CreateArrayTexture2D(64, 64, terrain->biome.entryCount, entryTextureFiles);

	for (U32 x = 0; x < terrain->widthInTiles; x++) {
		for (U32 y = 0; y < terrain->heightInTiles; y++) {
			U32 index0 = XYToIndex(terrain, x, y);
			U32 index1 = XYToIndex(terrain, x + 1, y);
			U32 index2 = XYToIndex(terrain, x + 1, y + 1);
			U32 index3 = XYToIndex(terrain, x, y + 1);

			//The min height is proably all we need
			F32 minHeight = terrain->heightmap[index0];
			minHeight = MathUtils::Min(terrain->heightmap[index1], minHeight);
			minHeight = MathUtils::Min(terrain->heightmap[index2], minHeight);
			minHeight = MathUtils::Min(terrain->heightmap[index3], minHeight);
			U32 id = GetBiomeEntryID(&terrain->biome, minHeight);

			terrain->tilemap[index0].biomeEntryID = id;
			auto& entry = terrain->biome.entries[id - 1];
			if (terrain->heightmap[index0] > entry.fadeBeginHeight ||
				terrain->heightmap[index1] > entry.fadeBeginHeight ||
				terrain->heightmap[index2] > entry.fadeBeginHeight ||
				terrain->heightmap[index3] > entry.fadeBeginHeight) {
				terrain->tilemap[index0].overlaps = true;
			}
			else {
				terrain->tilemap[index0].overlaps = false;
			}

			terrain->tilemap[index0].alpha0 = SampleAlpha(&terrain->biome, id, terrain->heightmap[index0]);
			terrain->tilemap[index0].alpha1 = SampleAlpha(&terrain->biome, id, terrain->heightmap[index1]);
			terrain->tilemap[index0].alpha2 = SampleAlpha(&terrain->biome, id, terrain->heightmap[index2]);
			terrain->tilemap[index0].alpha3 = SampleAlpha(&terrain->biome, id, terrain->heightmap[index3]);
		}
	}

	glDeleteTextures(1, &mapTextureID);
	mapTextureID = TerrainToTexture(terrain);
}

void GenerateHeightmap(F32* heightmap, U32 width, U32 height, U64 seed) {
	Random rng(seed);
	OpenSimplexNoise noise0(rng.Range(0, 1 << 16));
	for (U32 i = 0; i < width * height; i++) {
		U32 y = (i / width);
		U32 x = i - (y * height);
		heightmap[i] = noise0.FBM(x, y, 6, 0.006f, 0.5f);
	}

	OpenSimplexNoise noise1(rng.Range(0, 1 << 16));
	for (U32 i = 0; i < width * height; i++) {
		U32 y = (i / width);
		U32 x = i - (y * height);
		heightmap[i] += noise1.RidgedNoise(x, y, 6, 0.015f, 0.6f);
		heightmap[i] = MathUtils::Clamp(heightmap[i], -1.0f, 1.0f);
	}

	//OpenSimplexNoise low(time(NULL));
	//for (U32 i = 0; i < width * height; i++) {
	//	U32 y = (i / width);
	//	U32 x = i - height;
	//	heightmap[i] -= low.RidgedNoise(x, y, 6, 0.0006f, 0.5f) * 0.025f;
	//	heightmap[i] = MathUtils::Clamp(heightmap[i], -1.0f, 1.0f);
	//}
}

U32 CreateScatterMap(Vector2* positions, F32 x, F32 y, F32 width, F32 height, U32 rows, U32 cols) {
	Random rng(0);
	auto cellWidth = width / rows;
	auto cellHeight = height / cols;
	auto scatter = 0.4f;

	U32 count = 0;
	for (auto ix = 0; ix < cols; ix++) {
		for (auto iy = 0; iy < rows; iy++) {

			//positions[count++] = Vector2(x + (cellWidth * ix) + (rng.Range(-scatter, scatter) * cellWidth),
			//							 y + (cellHeight * iy) + (rng.Range(-scatter, scatter)*cellHeight));
			entities[count++].position = Vector2(x + (cellWidth * ix) + (rng.Range(-scatter, scatter) * cellWidth),
										 y + (cellHeight * iy) + (rng.Range(-scatter, scatter)*cellHeight));
		}
	}
	return count;
}

void Populate(Terrain2D* terrain) {
	Random rng;
	auto count = CreateScatterMap(entityPositions, 35, 27, 256, 256, 128, 128);
	assert(count < MAX_ENTITES);
	for (auto i = 0; i < count; i++) {
		//entityTexureID[i] = nullTextureID;
		entities[i].textureID = rng.Range(0, FOLIAGE_COUNT);
	}
	gEntityCount = count;
}

// Terrain is created in meters. Tile size might vary!
void CreateTerrain(Terrain2D* terrain, U32 widthInMeters, U32 heightInMeters) {
	terrain->widthInTiles = ceil(((F32)widthInMeters / (F32)TILE_SIZE_METERS));
	terrain->heightInTiles = ceil(((F32)heightInMeters / (F32)TILE_SIZE_METERS));
	size_t heightmapMemorySize = sizeof(F32) * ((terrain->widthInTiles + 1) * (terrain->heightInTiles + 1));
	size_t tilemapMemorySize = sizeof(Tile) * ((terrain->widthInTiles + 1) * (terrain->heightInTiles + 1));
	terrain->memblock = (U8*)malloc(heightmapMemorySize+tilemapMemorySize);
	terrain->heightmap = (F32*)terrain->memblock;
	terrain->tilemap = (Tile*)(terrain->memblock + heightmapMemorySize);
	GenerateHeightmap(terrain->heightmap, terrain->widthInTiles, terrain->heightInTiles, time(NULL));
	LoadBiome(terrain);
	Populate(terrain);
}

void DestroyTerrain(Terrain2D* terrain) {
	free(terrain->memblock);
}

void KeyCallback(int keycode, bool isDown) {
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[keycode] = isDown;
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
	if (app.IsKeyDown(KEY_W)) deltaPos.y += 1.0f * deltaTime;
	if (app.IsKeyDown(KEY_S)) deltaPos.y -= 1.0f * deltaTime;
	if (app.IsKeyDown(KEY_D)) deltaPos.x += 1.0f * deltaTime;
	if (app.IsKeyDown(KEY_A)) deltaPos.x -= 1.0f * deltaTime;
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

	auto halfViewportWidth = (gCameraTransform.size.x * 0.5f) * cameraZoom;
	auto halfViewportHeight = (gCameraTransform.size.y * 0.5f) * cameraZoom;
	auto left = floor((gCameraTransform.position.x - halfViewportWidth) / (F32)TILE_SIZE_METERS);
	auto right = ceil((gCameraTransform.position.x + halfViewportWidth) / TILE_SIZE_METERS);
	auto bottom = floor((gCameraTransform.position.y - halfViewportHeight) / TILE_SIZE_METERS);
	auto top = ceil((gCameraTransform.position.y + halfViewportHeight) / TILE_SIZE_METERS);
	U32 startX = left > 0 ? left : 0;
	U32 endX = right < gTerrain.widthInTiles ? right : gTerrain.widthInTiles - 1;
	U32 startY = bottom > 0 ? bottom : 0;
	U32 endY = top < gTerrain.heightInTiles ? top : gTerrain.heightInTiles - 1;

	static F32 waveAngle = 0.0f;
	static F32 waveAmp = 0.1f;
	waveAngle += deltaTime * 1.0f;
	if(waveAngle > RADIANS(360)) waveAngle = 0;
	glUseProgram(terrainShader.shaderProgramID);
	glUniform1f(terrainShader.waveAngleUniformLocation, waveAngle);


	BENCHMARK_START(draw);
	static bool drawMapTexture = true;
	static bool drawWater = true;
	static bool drawSand = true;
	if (drawMapTexture) {
		DEBUGDrawTexture(&gRenderGroup, mapTextureID, 0, 0, 512, 512, Color());
		DEBUGFlushGroup(&gRenderGroup);
	} else {
		glUseProgram(terrainShader.shaderProgramID);
		Matrix4 projection = TransformToOrtho(gCameraTransform, cameraZoom);
		glUniformMatrix4fv(terrainShader.projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);
		gRenderGroup.currentTextureID = gTerrain.biome.textureID;


		std::vector<U32> waterTiles;
		std::vector<U32> overlapWaterTiles;
		TileVertex vertices[4];

		auto terrain = &gTerrain;
		for (U32 x = startX; x <= endX; x++) {
			for (U32 y = startY; y <= endY; y++) {
				auto tileIndex = XYToIndex(&gTerrain, x, y);
				auto& tile = terrain->tilemap[tileIndex];
				if (tile.overlaps) {
					U32 id = tile.biomeEntryID - 1;
					if (id != 0) {
						if (id == terrain->biome.entryCount || id == terrain->biome.entryCount - 1) {
							overlapWaterTiles.push_back(XYToIndex(&gTerrain, x, y));
							continue;
						}
						auto& baseEntry = terrain->biome.entries[id - 1];
						U32 entryIndex = id - 1;
						vertices[0] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS)), Vector3(0.0f, 0.0f, entryIndex), baseEntry.color };
						vertices[1] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS)), Vector3(1.0f, 0.0f, entryIndex), baseEntry.color };
						vertices[2] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(1.0f, 1.0f, entryIndex), baseEntry.color };
						vertices[3] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(0.0f, 1.0f, entryIndex), baseEntry.color };

						DEBUGPushVertices(&gRenderBuffer, vertices, 4);
					}
				}

				//We still push our overlap on! unless .....
				if (tile.biomeEntryID == terrain->biome.entryCount || tile.biomeEntryID == terrain->biome.entryCount - 1) {
					waterTiles.push_back(XYToIndex(&gTerrain, x, y));
					continue;
				}

				if (!drawWater && tile.biomeEntryID == 2) continue;
				if (!drawSand && tile.biomeEntryID == 1) continue;
				U32 entryIndex = tile.biomeEntryID - 1;
				BiomeEntry& entry = gTerrain.biome.entries[entryIndex];
				vertices[0] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS)), Vector3(0.0f, 0.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha0) };
				vertices[1] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS)), Vector3(1.0f, 0.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha1) };
				vertices[2] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(1.0f, 1.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha2) };
				vertices[3] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(0.0f, 1.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha3) };
				DEBUGPushVertices(&gRenderBuffer, vertices, 4);
			}
		}
		glUniform1i(terrainShader.isWaterUniformLocation, 0);
		DEBUGDrawGroup(&gRenderBuffer);
		for (auto& tileIndex : waterTiles) {
			auto& tile = terrain->tilemap[tileIndex];
			U32 y = (tileIndex / terrain->widthInTiles);
			U32 x = tileIndex - (y * terrain->heightInTiles);
			U32 entryIndex = tile.biomeEntryID - 1;
			BiomeEntry& entry = gTerrain.biome.entries[entryIndex];
			vertices[0] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS)), Vector3(0.0f, 0.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha0) };
			vertices[1] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS)), Vector3(1.0f, 0.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha1) };
			vertices[2] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(1.0f, 1.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha2) };
			vertices[3] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(0.0f, 1.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha3) };
			DEBUGPushVertices(&gRenderBuffer, vertices, 4);
		}
		for (auto& tileIndex : overlapWaterTiles) {
			auto& tile = terrain->tilemap[tileIndex];
			U32 y = (tileIndex / terrain->widthInTiles);
			U32 x = tileIndex - (y * terrain->heightInTiles);
			U32 entryIndex = tile.biomeEntryID - 2;
			BiomeEntry& entry = gTerrain.biome.entries[entryIndex];
			vertices[0] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS)), Vector3(0.0f, 0.0f, entryIndex), entry.color };
			vertices[1] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS)), Vector3(1.0f, 0.0f, entryIndex), entry.color };
			vertices[2] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(1.0f, 1.0f, entryIndex), entry.color };
			vertices[3] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(0.0f, 1.0f, entryIndex), entry.color };
			DEBUGPushVertices(&gRenderBuffer, vertices, 4);
		}
	}
	glUniform1i(terrainShader.isWaterUniformLocation, 1);
	DEBUGDrawGroup(&gRenderBuffer);


	glUniform1i(terrainShader.isWaterUniformLocation, 0);

	glUseProgram(spriteProgramID);
	Matrix4 projection = TransformToOrtho(gCameraTransform, cameraZoom);
	glUniformMatrix4fv(glGetUniformLocation(spriteProgramID, "projection"), 1, GL_FALSE, &projection[0][0]);
	DEBUGBindGroup(&gRenderGroup);
	BENCHMARK_START(entityDrawTime);
	for (auto i = 0; i < gEntityCount; i++)
		//DEBUGDrawTexture(&gRenderGroup, entityTexureID[i], entityPositions[i], Vector2(1.0f, 1.0f), Color());
		DEBUGDrawTexture(&gRenderGroup, entities[i].textureID, entities[i].position, Vector2(4.0f, 4.0f), Color());
	BENCHMARK_END(entityDrawTime);
	DEBUGFillRect(&gRenderGroup, gPlayerTransform.position.x, gPlayerTransform.position.y, gPlayerTransform.size.x, gPlayerTransform.size.y, Color(0.0f, 1.0f, 0.0f, 1.0f));
	DEBUGFlushGroup(&gRenderGroup);
	BENCHMARK_END(draw);


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

	if (ImGui::Button("Reload Biome")) LoadBiome(&gTerrain);
	if (ImGui::Button("Reload Heightmap")) {
		GenerateHeightmap(gTerrain.heightmap, gTerrain.widthInTiles, gTerrain.heightInTiles, time(NULL));
		LoadBiome(&gTerrain);
	}


	ImGui::Begin("Profiler");
	static const uint32 resolution = 32;
	static uint32 cntr = 0;
	static float32 frameTimes[resolution];
	frameTimes[cntr] = deltaTime * 1000.0f;
	if (++cntr == resolution) cntr = 0;
	ImGui::Text((std::string("Tile draw Time: ") + std::to_string(benchmark_draw_time)).c_str());
	ImGui::Text((std::string("Entity DrawTime: " + std::to_string(benchmark_entityDrawTime_time))).c_str());
	ImGui::PlotLines("##MainLoop", frameTimes, resolution, cntr, "MainLoop", 0.0f, 100.0f, ImVec2(400, 75));
	ImGui::End();

	auto terrain = &gTerrain;
	ImGui::Begin("Tile Info");
	U32 tileX = (U32)floor((gPlayerTransform.position.x / (F32)TILE_SIZE_METERS));
	U32 tileY = (U32)floor((gPlayerTransform.position.y / (F32)TILE_SIZE_METERS));
	auto& tile = terrain->tilemap[XYToIndex(&gTerrain, tileX, tileY)];
	ImGui::Text((std::string("X:") + std::to_string(tileX) + std::string(" Y: ") + std::to_string(tileY)).c_str());
 	ImGui::Text((std::string("Height: ") + std::to_string(terrain->heightmap[XYToIndex(&gTerrain, tileX, tileY)])).c_str());
	ImGui::Text((std::string("Overlaps: ") + std::string(tile.overlaps ? "True" : "False")).c_str());

	ImGui::Text((std::string("Alpha0: ") + std::to_string(tile.alpha0)).c_str());
	ImGui::Text((std::string("Alpha1: ") + std::to_string(tile.alpha1)).c_str());
	ImGui::Text((std::string("Alpha2: ") + std::to_string(tile.alpha2)).c_str());
	ImGui::Text((std::string("Alpha3: ") + std::to_string(tile.alpha3)).c_str());
	ImGui::End();

	static char inputBuffer[256];
	ImGui::Begin("Chat / Console");
	ImGui::InputText("Chat", inputBuffer, 256);
	ImGui::End();
	
	static bool connected = false;
	static NetworkServer server;
	static NetworkClient client;
	ImGui::Begin("Network");
	if (!connected) {
		if (ImGui::Button("Start Server")) {
			NetworkInit(&server);
			connected = true;
		}
		if (ImGui::Button("Join Server")) {
			connected = true;
		}
	}
	else {

	}
	
	ImGui::End();

	//ImGui::Begin("Example: Fixed Overlay", 0, ImVec2(0, 0), 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	//ImGui::Text("Test");
	//ImGui::End();

	//ImGui::ShowTestWindow();
	//ImGui::ShowStyleEditor();
	ImGui::Render();

	app.EndFrame();
}

static U32 maxWidth;
static U32 maxHeight;
static U32 minWidth;
static U32 minHeight;
#include <SDL2/SDL_image.h>
#undef main
GLuint SizeTest(std::string filename) {
	SDL_Surface* image = IMG_Load(filename.c_str());
	if (image == nullptr) {
		LOG_ERROR("Could not open file: " << filename << " :: " << IMG_GetError());
		return 0;
	}

	minWidth = MathUtils::Min(minWidth, image->w);
	minHeight = MathUtils::Min(minHeight, image->h);
	maxWidth = MathUtils::Max(maxWidth, image->w);
	maxHeight = MathUtils::Max(maxHeight, image->h);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return textureID;
}



int main () {
	app.Create("Raptor ImGUI", 1280, 720, true);
	app.SetKeyCallback(KeyCallback);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ImGuiContextInit(&gImGuiContext);

	DEBUGCreateRenderGroup(&gRenderGroup, 4096);
	terrainShader.Create();
	spriteProgramID = DEBUGLoadShaderFromFile(ASSET("shaders/Sprite.vert"), ASSET("shaders/Sprite.frag"));

	nullTextureID = DEBUGLoadTexture(ASSET("textures/null.png"));
	for (auto i = 0; i < FOLIAGE_COUNT; i++) {
		foliageTextures[i] = SizeTest(std::string(ASSET_DIR) + std::string("textures/foliage/tree_") + std::to_string(i + 1) + std::string(".png"));
	}


	DEBUGCreateBufferGroup(&gRenderBuffer, sizeof(TileVertex), 65536);
	DEBUGAddAttribute(&gRenderBuffer, 0, 2, GL_FLOAT, sizeof(TileVertex), (GLvoid*)offsetof(TileVertex, position));
	DEBUGAddAttribute(&gRenderBuffer, 1, 3, GL_FLOAT, sizeof(TileVertex), (GLvoid*)offsetof(TileVertex, texCoord));
	DEBUGAddAttribute(&gRenderBuffer, 2, 4, GL_FLOAT, sizeof(TileVertex), (GLvoid*)offsetof(TileVertex, color));

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

	F32 viewportHeightInMeters = VIEWPORT_WIDTH_IN_METERS * (app.GetHeight() / app.GetWidth());
	gCameraTransform.size = Vector2(VIEWPORT_WIDTH_IN_METERS, viewportHeightInMeters);
	gPlayerTransform.size.x = 1.0f;
	gPlayerTransform.size.y = 1.0f;

	CreateTerrain(&gTerrain, 512, 512);
	mapTextureID = TerrainToTexture(&gTerrain);

#ifndef __EMSCRIPTEN__
	LOG_INFO("The main loop is running...");
	while (app.isRunning) {
		MainLoop();
	}

	DestroyTerrain(&gTerrain);

#else
	emscripten_set_main_loop(MainLoop, 0, 1);
#endif

	return 0;
}
