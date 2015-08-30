#include <string>
#include <sstream>
#include <malloc.h>
#include <fstream>

#include <GL/glew.h>

#include <Core/Application.hpp>
#include <Core/Audio.hpp>
#include <Core/Network.hpp>

#include <Base/Transform2D.hpp>

#include <Graphics/Color.hpp>
#include <Graphics/GLSLProgram.hpp>
#include <Graphics/GUI.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/DEBUGRenderer.hpp>

#include <Math/Geometry2D.hpp>
#include <Math/Noise.hpp>
#include <Math/Random.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>

#define TILE_SIZE_METERS 1
#define VIEWPORT_WIDTH_IN_METERS 30

// TODO
// Fix problems with the biome creation
// Create a biome editor
// Fix problems with the water
// Make Terrain texture match up with the actual terrain

#if 0
struct TileVertex {
	Vector2 position;
	Vector3 texCoord;
	Color color;
};
#endif

struct TileVertex {
	Vector2 position;
	Vector2 texCoord;
	Color color;
};

#if 0
struct BiomeEntry {
	std::string texture;
	F32 startHeight;
	F32 fadeBeginHeight;
	F32 fadeEndHeight;
	Color color;
};
#endif

template<class Archive>
void serialize(Archive& archive, Vector2& vector) {
	archive(cereal::make_nvp("X", vector.x),
			cereal::make_nvp("Y", vector.y));
}
#if 0
template<class Archive>
void serialize(Archive & archive, BiomeEntry& entry) {
	archive(cereal::make_nvp("Texture", entry.texture),
			cereal::make_nvp("StartHeight", entry.startHeight),
			cereal::make_nvp("FadeBegin", entry.fadeBeginHeight),
			cereal::make_nvp("FadeEnd", entry.fadeEndHeight),
			cereal::make_nvp("Color" , entry.color)
			);
}
#endif

#if 0
struct BiomeEntry {
	std::string texture;
	F32 transitionBegin;
	F32 transitionEnd;
	Color color;
};
#endif 

struct BiomeEntry {
	U32 atlasID;
	F32 transitionBegin;
	F32 transitionEnd;
	Color color;
};

struct Biome {
	TextureAtlas atlas;
	GLuint textureID;
	U32 entryCount;
	BiomeEntry* entries;
};

template<class Archive>
void serialize(Archive & archive, BiomeEntry& entry) {
	archive(cereal::make_nvp("AtlasID", entry.atlasID),
			cereal::make_nvp("TransitionBegin", entry.transitionBegin),
			cereal::make_nvp("TransitionEnd", entry.transitionEnd),
			cereal::make_nvp("Color" , entry.color)
	);
}

struct Tile {
	bool overlaps;	//We check this first... The following data is allways needed no mater what
	U32 entryID;
	F32 alpha0;
	F32 alpha1;
	F32 alpha2;
	F32 alpha3;
};

struct Terrain2D {
	U32 widthInTiles, heightInTiles;
	U8* memblock;
	F32* heightmap;
	Tile* tilemap;
	Biome biome;
};

Terrain2D gTerrain;

inline U32 XYToIndex(Terrain2D* terrain, uint32 x, uint32 y) {
	auto result = (y * terrain->widthInTiles) + x;
	return result;
}

inline U32 XYToArrayIndex(U32 x, U32 y, U32 stride) {
	auto result = (y * stride) + x;
	return result;
}

BiomeEntry* FindBiomeEntryForHeight(Biome* biome, F32 height) {
	for(auto i = 0; i < biome->entryCount; i++) {
		auto& entry = biome->entries[i];
		if(height <= entry.transitionEnd) {
			return &entry;
		}
	}
	return nullptr;
}

U32 GetBiomeEntryID(BiomeEntry* entry, Biome* biome) {
	for (auto i = 0; i < biome->entryCount; i++) {
		if (entry == &biome->entries[i]) {
			return i + 1;
		}
	}
	return 0;
}

// Applys a biome to a Terrain2D heightmap and tilemap
// Doesnt allocate memory
void ApplyBiome(Terrain2D* terrain, Biome* biome) {
	for (U32 y = 0; y < terrain->heightInTiles; y++) {
		for (U32 x = 0; x < terrain->widthInTiles; x++) {
			// Get the four courner indices of the current tile
			auto index0 = XYToArrayIndex(x, y, terrain->widthInTiles);
			auto index1 = XYToArrayIndex(x + 1, y, terrain->widthInTiles);
			auto index2 = XYToArrayIndex(x + 1, y + 1, terrain->widthInTiles);
			auto index3 = XYToArrayIndex(x, y + 1, terrain->widthInTiles);

			// Get the minimum height value of the four corners of the tile we are looking at
			auto minHeight = terrain->heightmap[index0];
			minHeight = MathUtils::Min(terrain->heightmap[index1], minHeight);
			minHeight = MathUtils::Min(terrain->heightmap[index2], minHeight);
			minHeight = MathUtils::Min(terrain->heightmap[index3], minHeight);
			auto entry = FindBiomeEntryForHeight(biome, minHeight);	// Get the coresponding biome entry based on the min height

			// If the any of the tiles corner heightmap values are greater than the transitionBegin
			// Of the biome entry than the tile must overlap another
			auto& tile = terrain->tilemap[index0];
			if (terrain->heightmap[index0] > entry->transitionBegin ||
				terrain->heightmap[index1] > entry->transitionBegin ||
				terrain->heightmap[index2] > entry->transitionBegin ||
				terrain->heightmap[index3] > entry->transitionBegin ){
				tile.overlaps = true;
			} else tile.overlaps = false;

			// Get an alpha value based on the heightValue between the tiles transition points
			auto sampleAlpha = [entry](F32 height) -> F32 {
				if (height < entry->transitionBegin) return 1.0f;
				if (height > entry->transitionEnd) return 0.0f;
				auto transDiff = entry->transitionEnd - entry->transitionBegin;
				if (transDiff == 0) return 1.0f;
				auto transHeight = entry->transitionEnd - height;
				auto result = (transHeight / transDiff);
				return result;
			};

			// Set the alpha values of the tile in the tilemap and the entryID it will use
			tile.entryID = GetBiomeEntryID(entry, biome);
			tile.alpha0 = sampleAlpha(terrain->heightmap[index0]);
			tile.alpha1 = sampleAlpha(terrain->heightmap[index1]);
			tile.alpha2 = sampleAlpha(terrain->heightmap[index2]);
			tile.alpha3 = sampleAlpha(terrain->heightmap[index3]);
		}
	}
}

DEBUGBufferGroup gRenderBuffer;
DEBUGRenderGroup gRenderGroup;

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

struct Terrain2DShaderProgram {
	GLuint shaderProgramID;
	GLint projectionUniformLocation;
	GLint isWaterUniformLocation;
	GLint waveAngleUniformLocation;
} gTerrainShader;
GLuint spriteProgramID;

GLuint gTerrainTextureID;

TextureAtlas gTextureAtlas;
GLuint gFoliageAtlasTextureID;


GUIContext gGuiContext;

Transform2D gCameraTransform;
Transform2D gPlayerTransform;

GLuint nullTextureID;

Mix_Chunk* soundEffects[8];
Mix_Music* music;

#define MAX_ENTITES 100000
U32 gEntityCount;

Transform2D gEntities[MAX_ENTITES];
U32 gEntityTextures[MAX_ENTITES];

#define FOLIAGE_COUNT 47

struct TextureCoord2D{
	Vector2 uv[4];
};

bool gSoundEnabled = false;
bool gMusicEnabled = false;

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
		U32 y = index / terrain->widthInTiles;
		U32 x =  index - (y * terrain->widthInTiles);
		Color color;
		if(IsPointInCircle(x, y, terrain->widthInTiles * 0.5f, terrain->heightInTiles * 0.5f, terrain->widthInTiles * 0.5f)) {
			color = terrain->biome.entries[terrain->tilemap[index].entryID - 1].color;
			color = Color(color.r, color.g, color.b, terrain->tilemap[index].alpha0);
			auto result = BlendOverlay(color, terrain->biome.entries[terrain->tilemap[index].entryID].color);
			color = Color(result.r, result.g, result.b, color.a);
		} else {
			color = { 0.0f, 0.0f, 0.0f, 0.0f };
		}

		pixels[i + 0] = color.r * 255;
		pixels[i + 1] = color.g * 255;
		pixels[i + 2] = color.b * 255;
		pixels[i + 3] = color.a * 255;

	}
	auto result = CreateTextureFromPixels(terrain->widthInTiles, terrain->heightInTiles, pixels);
	delete[] pixels;
	return result;
}

#if 0
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


void InitBiome(Terrain2D* terrain) {
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

	glDeleteTextures(1, &gTerrainTextureID);
	gTerrainTextureID = TerrainToTexture(terrain);
}

#endif

void LoadBiomeFromFile(Biome* biome) {
	{
		std::ifstream is(ASSET("biome.json"));
		cereal::JSONInputArchive archive(is);
		archive(cereal::make_nvp("Entries", *biome));
	}
}

void GenerateHeightmap(F32* heightmap, U32 width, U32 height, U64 seed) {
	Random rng(seed);
	OpenSimplexNoise noise0(rng.Range(0, 1 << 16));
	for (U32 i = 0; i < width * height; i++) {
		U32 y = (i / width);
		U32 x = i - (y * height);
		heightmap[i] = noise0.FBM(x, y, 6, 0.006f, 0.5f);
	}

	//OpenSimplexNoise noise1(rng.Range(0, 1 << 16));
	//for (U32 i = 0; i < width * height; i++) {
	//	U32 y = (i / width);
	//	U32 x = i - (y * height);
	//	heightmap[i] += noise1.RidgedNoise(x, y, 6, 0.015f, 0.6f);
	//	heightmap[i] = MathUtils::Clamp(heightmap[i], -1.0f, 1.0f);
	//}

	//OpenSimplexNoise low(time(NULL));
	//for (U32 i = 0; i < width * height; i++) {
	//	U32 y = (i / width);
	//	U32 x = i - height;
	//	heightmap[i] -= low.RidgedNoise(x, y, 6, 0.0006f, 0.5f) * 0.025f;
	//	heightmap[i] = MathUtils::Clamp(heightmap[i], -1.0f, 1.0f);
	//}
}

U32 CreateScatterMap(F32 x, F32 y, F32 width, F32 height, U32 rows, U32 cols) {
	Random rng(0);
	auto cellWidth = width / rows;
	auto cellHeight = height / cols;
	auto scatter = 0.4f;

	U32 count = 0;
	for (auto ix = 0; ix < cols; ix++) {
		for (auto iy = 0; iy < rows; iy++) {
			gEntities[count].position = Vector2(x + (cellWidth * ix) + (rng.Range(-scatter, scatter) * cellWidth),
										 y + (cellHeight * iy) + (rng.Range(-scatter, scatter)*cellHeight));
			count++;
		}
	}
	return count;
}

void Populate(Terrain2D* terrain) {
	Random rng;
	auto count = CreateScatterMap(35, 27, 256, 256, 64, 64);
	assert(count < MAX_ENTITES);
	for (auto i = 0; i < count; i++) {
		auto texIndex = rng.Range(0, FOLIAGE_COUNT);
		gEntityTextures[i] = texIndex;
		auto width = gTextureAtlas.regions[texIndex].right - gTextureAtlas.regions[texIndex].x;
		auto height = gTextureAtlas.regions[texIndex].top - gTextureAtlas.regions[texIndex].y;
		gEntities[i].size = Vector2(width, height)*6;

	}
	gEntityCount = count;
}

void CreateDebugBiome(Biome* biome) {
	std::vector<BiomeEntry> entries;
	entries.push_back({ 2, -0.5f, 0.0f, Color(0.0f, 0.0f, 1.0f, 1.0f)});
	entries.push_back({ 0, 1.0f, 1.0f, Color(0.0f, 1.0f, 0.0f, 1.0f)});

	biome->entryCount = entries.size();
	biome->entries = new BiomeEntry[entries.size()];
	for (auto i = 0; i < biome->entryCount; i++) {
		biome->entries[i] = entries[i];
	}

	LoadTextureAtlasFromFile(&biome->atlas, "textures/biome/biome.atlas");
	biome->textureID = CreateTextureFromPixels(biome->atlas.width, biome->atlas.height, biome->atlas.pixels);
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
	//LoadBiomeFromFile(&terrain->biome);
	CreateDebugBiome(&terrain->biome);
	ApplyBiome(terrain, &terrain->biome);
	Populate(terrain);
}

void DestroyTerrain(Terrain2D* terrain) {
	free(terrain->memblock);
}
enum MovementFlags {
	MOVE_FOWARD =		1 << 0,
	MOVE_BACKWARD =		1 << 1,
	MOVE_LEFT =			1 << 2,
	MOVE_RIGHT =		1 << 3,
	MOVE_SPRINT =		1 << 4,
	MOVE_DEBUG_HYPERSPRINT = 1 << 5,
};

//For now we can use this to do stuff and things!
struct RigidBody2D {
	Vector2 velocity;
};

void ProcessPlayerMovement(Transform2D* transform, RigidBody2D* body, U32 moveFlags, F64 deltaTime) {
	static const F64 ACCELLERATION = 35.0;
	static const F64 MAX_VELOCITY = 3.0f;
	static const F64 SPRINT_MULTIPLIER = 2.0f;
	static const F64 DRAG_ACCELLERATION = 45.0f;
	auto maxVelocity = MAX_VELOCITY;

	bool isMovingVertical = false;
	bool isMovingHorizontal = false;
	if (moveFlags & MOVE_FOWARD) {
		body->velocity.y += (ACCELLERATION * deltaTime);
		isMovingVertical = true;
	}
	if (moveFlags & MOVE_BACKWARD) {
		body->velocity.y -= (ACCELLERATION * deltaTime);
		isMovingVertical = true;
	}
	if (moveFlags & MOVE_RIGHT) {
		body->velocity.x += (ACCELLERATION * deltaTime);
		isMovingHorizontal = true;
	}
	if (moveFlags & MOVE_LEFT) {
		body->velocity.x -= (ACCELLERATION * deltaTime);
		isMovingHorizontal = true;
	}
	if (moveFlags & MOVE_SPRINT) {
		maxVelocity *= SPRINT_MULTIPLIER;
	}
	if (moveFlags & MOVE_DEBUG_HYPERSPRINT) {
		maxVelocity *= SPRINT_MULTIPLIER * 5;
	}
	auto mag = body->velocity.Magnitude();
	if (mag > maxVelocity) {
		body->velocity.Normalize();
		body->velocity *= maxVelocity;
	}
	if (!isMovingHorizontal && !MathUtils::EpsilonEquals(body->velocity.x, 0.0f)) {
		if (abs(body->velocity.x) - (DRAG_ACCELLERATION * deltaTime) < 0) body->velocity.x = 0;
		else body->velocity.x += ((body->velocity.x > 0) ? -DRAG_ACCELLERATION : DRAG_ACCELLERATION) * deltaTime;
	}
	if (!isMovingVertical && !MathUtils::EpsilonEquals(body->velocity.y, 0.0f)) {
		if (abs(body->velocity.y) - (DRAG_ACCELLERATION * deltaTime) < 0) body->velocity.y = 0;
		else body->velocity.y += ((body->velocity.y > 0) ? -DRAG_ACCELLERATION : DRAG_ACCELLERATION) * deltaTime;
	}
	transform->position += body->velocity * deltaTime;
}

static U32 gMoveFlags;
void KeyCallback(int keycode, bool isDown) {
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[keycode] = isDown;
	if (keycode == KEY_LCTRL) io.KeyCtrl = isDown;

	switch (keycode) {
	case KEY_W: isDown ? gMoveFlags |= MOVE_FOWARD : gMoveFlags &= ~MOVE_FOWARD; break;
	case KEY_S: isDown ? gMoveFlags |= MOVE_BACKWARD : gMoveFlags &= ~MOVE_BACKWARD; break;
	case KEY_A: isDown ? gMoveFlags |= MOVE_LEFT : gMoveFlags &= ~MOVE_LEFT; break;
	case KEY_D: isDown ? gMoveFlags |= MOVE_RIGHT : gMoveFlags &= ~MOVE_RIGHT; break;
	case KEY_LSHIFT: isDown ? gMoveFlags |= MOVE_SPRINT : gMoveFlags &= ~MOVE_SPRINT; break;
	case KEY_LCTRL: isDown ? gMoveFlags |= MOVE_DEBUG_HYPERSPRINT : gMoveFlags &= ~MOVE_DEBUG_HYPERSPRINT; break;
	}
}

inline F32 Lerp(F32 from, F32 to, F32 t) {
	auto result = ((1.0f - t) * from) + (t*to);
	return result;
}

inline Vector2 Lerp(Vector2& from, Vector2& to, F32 t) {
	auto result = ((1.0f - t) * from) + (t * to);
	return result;
}

#if 0
void DrawTerrain(Terrain2D* terrain, U32 startX, U32 startY, U32 endX, U32 endY) {
	DEBUGBindGroup(&gRenderBuffer);
	TileVertex vertices[4];
	for (auto y = startY; y < endY; y++) {
		for (auto x = startX; x < endX; x++) {
			
		}
	}
	DEBUGDrawGroup(&gRenderBuffer);
}
#endif

#if 1
void DrawTerrain(Terrain2D* terrain, U32 startX, U32 startY, U32 endX, U32 endY) {
	DEBUGBindGroup(&gRenderBuffer);
	TileVertex vertices[4];
	TextureAtlas atlas = terrain->biome.atlas;
	AtlasRegion* regions = terrain->biome.atlas.regions;
	for (auto y = startY; y < endY; y++) {
		for (auto x = startX; x < endX; x++) {
			auto tileIndex = XYToArrayIndex(x, y, terrain->widthInTiles);
			auto& tile = terrain->tilemap[tileIndex];
			if (tile.overlaps) {
				auto entryIndex = tile.entryID;	//We dont minus one because we want the actual index of the one above the tile
				assert(entryIndex < terrain->biome.entryCount);	//Last biome entry transitions must be set to 1.0 Biome is not configured correctly! If this is the last biome entry then this is a problem and the 
				BiomeEntry& entry = terrain->biome.entries[entryIndex];
				Color color = entry.color;
				AtlasRegion& region = regions[entry.atlasID];
				vertices[0] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS)), Vector2(region.x, region.y), color };
				vertices[1] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS)), Vector2(region.right, region.y), color };
				vertices[2] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector2(region.right, region.top), color };
				vertices[3] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector2(region.x, region.top), color };
				DEBUGPushVertices(&gRenderBuffer, vertices, 4);
			}
			auto entryIndex = tile.entryID - 1;
			BiomeEntry& entry = terrain->biome.entries[entryIndex];
			AtlasRegion& region = regions[entry.atlasID];
			vertices[0] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS)), Vector2(region.x, region.y), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha0) };
			vertices[1] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS)), Vector2(region.right, region.y), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha1) };
			vertices[2] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector2(region.right, region.top), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha2) };
			vertices[3] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS) + TILE_SIZE_METERS),Vector2(region.x, region.top), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha3) };
			DEBUGPushVertices(&gRenderBuffer, vertices, 4);
		}
	}
	DEBUGDrawGroup(&gRenderBuffer);
}
#endif

void MainLoop (Application* app) {
	BENCHMARK_START(mainLoop);
	F64 deltaTime = app->GetDeltaTime();

	//NOTE @CameraControls
	static const F32 ZOOM_SPEED = 0.1f;
	static const F32 ZOOM_STEP = 0.1f;
	static const F32 ZOOM_MIN = 0.4f;
	static const F32 ZOOM_MAX = FLT_MAX;
	static F32 currentCameraZoom = 1.0f;
	static F32 targetCameraZoom = 1.0f;
	auto mouseWheel = app->GetMouseWheel();
	auto zoomStep = (currentCameraZoom * ZOOM_STEP);
	if(mouseWheel) {
		targetCameraZoom -= mouseWheel * zoomStep;
		targetCameraZoom = MathUtils::Clamp(targetCameraZoom, ZOOM_MIN, ZOOM_MAX);
	}

	if (!MathUtils::EpsilonEquals(targetCameraZoom, currentCameraZoom)) {
		auto currentZoomSpeed = (currentCameraZoom * ZOOM_STEP) * 5;	//A single zoom step takes 1/5th of a second from the zoom step above the targetCameraZoom Regardless of how many steps are in between
		auto nextCameraZoom = Lerp(currentCameraZoom, targetCameraZoom, 0.1f);
		nextCameraZoom = MathUtils::Clamp(nextCameraZoom, ZOOM_MIN, ZOOM_MAX);
		currentCameraZoom = nextCameraZoom;
	}

	const float32 PAN_SPEED = 0.05f;
	static float64 cursorX = app->GetCursorX();
	static float64 cursorY = app->GetCursorY();
	float64 dx = app->GetCursorX() - cursorX;
	float64 dy = app->GetCursorY() - cursorY;
	cursorX = app->GetCursorX();
	cursorY = app->GetCursorY();
	if(app->IsButtonDown(MOUSE_BUTTON_RIGHT)) {
		gCameraTransform.position.x -= dx * PAN_SPEED;
		gCameraTransform.position.y += dy * PAN_SPEED;
	}

	static RigidBody2D body;
	ProcessPlayerMovement(&gPlayerTransform, &body, gMoveFlags, deltaTime);

	auto cameraTarget = (gPlayerTransform.position + (gPlayerTransform.size * 0.5f));
	gCameraTransform.position = Lerp(gCameraTransform.position, cameraTarget, 0.75f);


	auto halfViewportWidth = (gCameraTransform.size.x * 0.5f) * currentCameraZoom;
	auto halfViewportHeight = (gCameraTransform.size.y * 0.5f) * currentCameraZoom;
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

	//// NOTE @SpaceBackground
	Matrix4 screenProjection = Matrix4::Ortho(0.0f, app->GetWidth(), 0.0f, app->GetHeight(), 1.0);
	static GLuint spaceTextureID = CreateTextureFromFile("skybox/space/left.png");
	static GLint shaderProgramProjectionLoc = GetUniformLocation(spriteProgramID, "projection");
	//if (currentCameraZoom > 15.0) {
	//	glUseProgram(spriteProgramID);
	//	glUniformMatrix4fv(shaderProgramProjectionLoc, 1, GL_FALSE, &screenProjection[0][0]);
	//	DEBUGBindGroup(&gRenderGroup);
	//	DEBUGDrawTexture(&gRenderGroup, spaceTextureID, Vector2(0, 0), Vector2(app->GetWidth(), app->GetHeight()), Color());
	//	DEBUGFlushGroup(&gRenderGroup);
	//}


	static auto counterFrequency = SDL_GetPerformanceFrequency() / 1000.0;
	auto drawStartTime = SDL_GetPerformanceCounter();
	auto drawStartcycles = __rdtsc();
	Matrix4 projection = TransformToOrtho(gCameraTransform, currentCameraZoom);
	if (currentCameraZoom > 4.0f) {
		glUseProgram(spriteProgramID);
		glUniformMatrix4fv(shaderProgramProjectionLoc, 1, GL_FALSE, &projection[0][0]);
		DEBUGBindGroup(&gRenderGroup);
		DEBUGDrawTexture(&gRenderGroup, gTerrainTextureID, Vector2(0, 0), Vector2(gTerrain.widthInTiles, gTerrain.heightInTiles), Color());
		DEBUGFlushGroup(&gRenderGroup);
	} else {
		glUseProgram(gTerrainShader.shaderProgramID);
		glUniform1f(gTerrainShader.waveAngleUniformLocation, waveAngle);
		glUniformMatrix4fv(gTerrainShader.projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTerrain.biome.textureID);
		DrawTerrain(&gTerrain, startX, startY, endX, endY);
		glBindTexture(GL_TEXTURE_2D, 0);
		gRenderGroup.currentTextureID = gTerrain.biome.textureID;
	}



	//// NOTE @Atmosphere
	static GLuint atmoProgram = LoadShaderFromFile("shaders/Atmosphere2D.vert", "shaders/Atmosphere2D.frag");
	static GLuint atmoProjectionLoc = GetUniformLocation(atmoProgram, "projection");
	static const F32 atmosphereThickness = 45.0f;
	//if (currentCameraZoom > 15.0) {
	//	glUseProgram(atmoProgram);
	//	glUniformMatrix4fv(atmoProjectionLoc, 1, GL_FALSE, &projection[0][0]);
	//	DEBUGFillRect(&gRenderGroup, -atmosphereThickness, -atmosphereThickness, gTerrain.widthInTiles + (atmosphereThickness * 2), gTerrain.heightInTiles + (atmosphereThickness * 2), Color());
	//	DEBUGFlushGroup(&gRenderGroup);
	//}



		//BENCHMARK_START(draw);
		//static bool drawMapTexture = false;
		//static bool drawWater = true;
		//static bool drawSand = true;
		//gRenderGroup.currentTextureID = gTerrain.biome.textureID;

	//	std::vector<U32> waterTiles;
	//	std::vector<U32> overlapWaterTiles;
	//	TileVertex vertices[4];
	//	auto terrain = &gTerrain;
	//	for (U32 x = startX; x <= endX; x++) {
	//		for (U32 y = startY; y <= endY; y++) {
	//			auto tileIndex = XYToIndex(&gTerrain, x, y);
	//			auto& tile = terrain->tilemap[tileIndex];
	//			if (tile.overlaps) {
	//				U32 id = tile.entryID;
	//				if (id != 0) {
	//					if (id == terrain->biome.entryCount || id == terrain->biome.entryCount - 1) {
	//						overlapWaterTiles.push_back(XYToIndex(&gTerrain, x, y));
	//						continue;
	//					}
	//					auto& baseEntry = terrain->biome.entries[id - 1];
	//					U32 entryIndex = id - 1;
	//					vertices[0] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS)), Vector3(0.0f, 0.0f, entryIndex), baseEntry.color };
	//					vertices[1] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS)), Vector3(1.0f, 0.0f, entryIndex), baseEntry.color };
	//					vertices[2] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(1.0f, 1.0f, entryIndex), baseEntry.color };
	//					vertices[3] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(0.0f, 1.0f, entryIndex), baseEntry.color };

	//					DEBUGPushVertices(&gRenderBuffer, vertices, 4);
	//				}
	//			}

	//			//We still push our overlap on! unless .....
	//			if (tile.entryID == terrain->biome.entryCount || tile.entryID == terrain->biome.entryCount - 1) {
	//				waterTiles.push_back(XYToIndex(&gTerrain, x, y));
	//				continue;
	//			}

	//			if (!drawWater && tile.entryID == 2) continue;
	//			if (!drawSand && tile.entryID == 1) continue;
	//			U32 entryIndex = tile.entryID - 1;
	//			BiomeEntry& entry = gTerrain.biome.entries[entryIndex];
	//			vertices[0] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS)), Vector3(0.0f, 0.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha0) };
	//			vertices[1] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS)), Vector3(1.0f, 0.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha1) };
	//			vertices[2] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(1.0f, 1.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha2) };
	//			vertices[3] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(0.0f, 1.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha3) };
	//			DEBUGPushVertices(&gRenderBuffer, vertices, 4);
	//		}
	//	}
	//	glUniform1i(gTerrainShader.isWaterUniformLocation, 0);
	//	DEBUGDrawGroup(&gRenderBuffer);
	//	for (auto& tileIndex : waterTiles) {
	//		auto& tile = terrain->tilemap[tileIndex];
	//		U32 y = (tileIndex / terrain->widthInTiles);
	//		U32 x = tileIndex - (y * terrain->heightInTiles);
	//		U32 entryIndex = tile.entryID - 1;
	//		BiomeEntry& entry = gTerrain.biome.entries[entryIndex];
	//		vertices[0] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS)), Vector3(0.0f, 0.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha0) };
	//		vertices[1] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS)), Vector3(1.0f, 0.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha1) };
	//		vertices[2] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(1.0f, 1.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha2) };
	//		vertices[3] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(0.0f, 1.0f, entryIndex), Color(entry.color.r, entry.color.g, entry.color.b, tile.alpha3) };
	//		DEBUGPushVertices(&gRenderBuffer, vertices, 4);
	//	}
	//	for (auto& tileIndex : overlapWaterTiles) {
	//		auto& tile = terrain->tilemap[tileIndex];
	//		U32 y = (tileIndex / terrain->widthInTiles);
	//		U32 x = tileIndex - (y * terrain->heightInTiles);
	//		U32 entryIndex = tile.entryID - 2;
	//		BiomeEntry& entry = gTerrain.biome.entries[entryIndex];
	//		vertices[0] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS)), Vector3(0.0f, 0.0f, entryIndex), entry.color };
	//		vertices[1] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS)), Vector3(1.0f, 0.0f, entryIndex), entry.color };
	//		vertices[2] = { Vector2((x * TILE_SIZE_METERS) + TILE_SIZE_METERS, (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(1.0f, 1.0f, entryIndex), entry.color };
	//		vertices[3] = { Vector2((x * TILE_SIZE_METERS), (y * TILE_SIZE_METERS) + TILE_SIZE_METERS), Vector3(0.0f, 1.0f, entryIndex), entry.color };
	//		DEBUGPushVertices(&gRenderBuffer, vertices, 4);
	//	}

	//	glUniform1i(gTerrainShader.isWaterUniformLocation, 1);
	//	DEBUGDrawGroup(&gRenderBuffer);
	//	glUniform1i(gTerrainShader.isWaterUniformLocation, 0);

	//	if (currentCameraZoom > 1.0f) {
	//		static GLuint shaderProgramProjectionLoc = GetUniformLocation(spriteProgramID, "projection");
	//		glUseProgram(spriteProgramID);
	//		glUniformMatrix4fv(shaderProgramProjectionLoc, 1, GL_FALSE, &projection[0][0]);
	//		DEBUGBindGroup(&gRenderGroup);
	//		auto fade = (currentCameraZoom - 1.0f) / 3.0f;
	//		DEBUGDrawTexture(&gRenderGroup, gTerrainTextureID, Vector2(0, 0), Vector2(gTerrain.widthInTiles, gTerrain.heightInTiles), Color(1.0, 1.0, 1.0, fade));
	//		DEBUGFlushGroup(&gRenderGroup);
	//	}
	//	BENCHMARK_END(draw);
	//}

	//@ENTITY DRAW

	BENCHMARK_START(entityDrawTime);
	glUseProgram(spriteProgramID);
	glUniformMatrix4fv(shaderProgramProjectionLoc, 1, GL_FALSE, &projection[0][0]);
	DEBUGBindGroup(&gRenderGroup);
	gRenderGroup.currentTextureID = gFoliageAtlasTextureID;
	Vertex2D verts[4];
	for (auto i = 0; i < gEntityCount; i++) {
		auto& transform = gEntities[i];
		auto& region = gTextureAtlas.regions[gEntityTextures[i]];
		verts[0] = { Vector2(transform.position.x, transform.position.y), Vector2((gTextureAtlas.regions[gEntityTextures[i]]).x, (gTextureAtlas.regions[gEntityTextures[i]]).y), Color(1.0f, 0.25f, 0.25f, 1.0f) };
		verts[1] = { Vector2(transform.position.x + transform.size.x, transform.position.y), Vector2((gTextureAtlas.regions[gEntityTextures[i]]).right, (gTextureAtlas.regions[gEntityTextures[i]]).y), Color(1.0f, 0.25f, 0.25f, 1.0f) };
		verts[2] = { Vector2(transform.position.x + transform.size.x, transform.position.y + transform.size.y), Vector2((gTextureAtlas.regions[gEntityTextures[i]]).right, (gTextureAtlas.regions[gEntityTextures[i]]).top), Color(1.0f, 0.25f, 0.25f, 1.0f) };
		verts[3] = { Vector2(transform.position.x, transform.position.y + transform.size.y), Vector2((gTextureAtlas.regions[gEntityTextures[i]]).x, (gTextureAtlas.regions[gEntityTextures[i]]).top), Color(1.0f, 0.25f, 0.25f, 1.0f) };
		DEBUGPushVertices(&gRenderGroup, verts, 4);
	}

	static Vector2 vehiclePosition = Vector2(512, 512);
	static Vector2 vehicleSize = Vector2(2.0f, 2.0f);
	static GLuint vehicleTextureID = CreateTextureFromFile("textures/vehicle.png");
	DEBUGBindGroup(&gRenderGroup);
	DEBUGDrawTexture(&gRenderGroup, vehicleTextureID, vehiclePosition, vehicleSize, Color(1.0, 1.0, 1.0, 1.0));
	DEBUGFlushGroup(&gRenderGroup);

	DEBUGFillRect(&gRenderGroup, gPlayerTransform.position.x, gPlayerTransform.position.y, gPlayerTransform.size.x, gPlayerTransform.size.y, Color(0.0f, 1.0f, 0.0f, 1.0f));
	DEBUGFlushGroup(&gRenderGroup);
	BENCHMARK_END(entityDrawTime);

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
	auto drawTime = SDL_GetPerformanceCounter() - drawStartTime;
	auto drawCycles = __rdtsc() - drawStartcycles;

	//NOTE @CloudRendering
	static GLuint cloudShaderID = LoadShaderFromFile("shaders/clouds.vert", "shaders/clouds.frag");
	static GLint cloudShaderProjectionloc = GetUniformLocation(cloudShaderID, "projection");
	static GLint cloudShaderTimeLoc = GetUniformLocation(cloudShaderID, "cloudTime");
	static F32 cloudTime = 1;
	cloudTime += (F32)(5.0f * deltaTime);
	if (currentCameraZoom > 2.5f) {
		glUseProgram(cloudShaderID);
		glUniformMatrix4fv(cloudShaderProjectionloc, 1, GL_FALSE, &projection[0][0]);
		glUniform1f(cloudShaderTimeLoc, cloudTime);
		DEBUGBindGroup(&gRenderGroup);
		DEBUGFillRect(&gRenderGroup, 0, 0, gTerrain.widthInTiles, gTerrain.heightInTiles, Color());
		DEBUGFlushGroup(&gRenderGroup);
	}

	//static GLuint shipTextureID = CreateTextureFromFile("textures/ship.png");
	//if (currentCameraZoom > 6.0f) {
	//	glUseProgram(spriteProgramID);
	//	DEBUGBindGroup(&gRenderGroup);
	//	DEBUGDrawTexture(&gRenderGroup, shipTextureID, 460.0f, 460.0f, 500.0f, 500.0f, Color());
	//	DEBUGFlushGroup(&gRenderGroup);
	//}

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
	GUIBeginFrame(&gGuiContext, app);
	std::stringstream stream;
	stream << "Player Position: " << gPlayerTransform.position;
	ImGui::Text(stream.str().c_str());
	stream.str("");
	stream << "Camera Zoom: " << currentCameraZoom;
	ImGui::Text(stream.str().c_str());
	stream.str("");
	stream << "Tiles Drawn: " << (endX - startX) * (endY - startY);
	ImGui::Text(stream.str().c_str());
	stream.str("");
	stream << "DeltaTime: " << app->GetDeltaTime() << "  FPS: " << 1000.0f / app->GetDeltaTime();
	ImGui::Text(stream.str().c_str());
	stream.str("");
	stream << "DrawCalls: " << gRenderGroup.drawCalls;
	ImGui::Text(stream.str().c_str());
	gRenderGroup.drawCalls = 0;

	ImGui::Checkbox("Draw grid", &drawGrid);
	//ImGui::Checkbox("Draw base color", &drawMapTexture);
	//ImGui::Checkbox("Draw water", &drawWater);
	//ImGui::Checkbox("Draw sand", &drawSand);

	//@BUTTONS
	if (ImGui::Button("Reload Shader")) {
        glDeleteProgram(gTerrainShader.shaderProgramID);
        gTerrainShader.shaderProgramID = LoadShaderFromFile("shaders/Terrain2DVertex.glsl", "shaders/Terrain2DFragment.glsl");
        gTerrainShader.projectionUniformLocation = GetUniformLocation(gTerrainShader.shaderProgramID, "projection");
        gTerrainShader.isWaterUniformLocation = GetUniformLocation(gTerrainShader.shaderProgramID, "isWater");
        gTerrainShader.waveAngleUniformLocation = GetUniformLocation(gTerrainShader.shaderProgramID, "waveAngle");

		glDeleteProgram(spriteProgramID);
		spriteProgramID = LoadShaderFromFile("shaders/Sprite.vert", "shaders/Sprite.frag");
	}

	if (ImGui::Button("ReloadCloudShader")) {
		glDeleteProgram(cloudShaderID);
		cloudShaderID = LoadShaderFromFile("shaders/clouds.vert", "shaders/clouds.frag");
	}


	if (ImGui::Button("ReloadAtmosphereShader")) {
		glDeleteProgram(atmoProgram);
		atmoProgram = LoadShaderFromFile("shaders/Atmosphere2D.vert", "shaders/Atmosphere2D.frag");
	}


	if (ImGui::Button("Reload Biome From File"))  {
		LoadBiomeFromFile(&gTerrain.biome);
		ApplyBiome(&gTerrain, &gTerrain.biome);
	}
	if (ImGui::Button("Reload Heightmap")) {
		GenerateHeightmap(gTerrain.heightmap, gTerrain.widthInTiles, gTerrain.heightInTiles, time(NULL));
		LoadBiomeFromFile(&gTerrain.biome);
	}

	ImGui::Begin("Biome Builder");
	auto terrain = &gTerrain;
	if (ImGui::Button("Refresh Biome")) {
		ApplyBiome(&gTerrain, &gTerrain.biome);
	}
	ImGui::PushID("biomeconfig");
	auto& biome = terrain->biome;
	for (auto i = 0; i < terrain->biome.entryCount; i++) {
		ImGui::PushID(i);
		ImGui::SliderFloat("Transition Begin", &terrain->biome.entries[i].transitionBegin, i > 0 ? biome.entries[i - 1].transitionEnd : -1.0f, biome.entries[i].transitionEnd);
		ImGui::SliderFloat("Transition End", &terrain->biome.entries[i].transitionEnd, biome.entries[i].transitionBegin, 1.0f);
		ImGui::ColorEdit4("Color", &terrain->biome.entries[i].color.r, true);
		ImGui::Separator();
		ImGui::PopID();
	}
	ImGui::PopID();
	ImGui::End();

	ImGui::Image((ImTextureID)gTerrain.biome.textureID, ImVec2(512, 512));

	ImGui::Begin("Profiler");
	static const uint32 resolution = 32;
	static uint32 cntr = 0;
	static F32 frameTimes[resolution];
	frameTimes[cntr] = (F32)(deltaTime * 1000.0f);
	if (++cntr == resolution) cntr = 0;
	ImGui::Text("All Draw Time %f", ((F64)drawTime / (F64)counterFrequency));
	ImGui::Text("All Draw CPU Cycles %d", drawCycles);

	//ImGui::Text((std::string("Tile draw Time: ") + std::to_string(benchmark_draw_time)).c_str());
	ImGui::Text((std::string("Entity DrawTime: " + std::to_string(benchmark_entityDrawTime_time))).c_str());
	ImGui::PlotLines("##MainLoop", frameTimes, resolution, cntr, "MainLoop", 0.0f, 100.0f, ImVec2(400, 75));
	ImGui::End();

	ImGui::Begin("Tile Info", nullptr, ImGuiWindowFlags_NoResize);
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
	GUIEndFrame();
}

#if 1
#undef main
int main () {
	Application app("Inhabited", 1280, 720, true);
	app.SetKeyCallback(KeyCallback);

	GUIContextInit(&gGuiContext, app.GetWidth(), app.GetHeight());
	GUISetDisplaySize(&gGuiContext, app.GetWidth(), app.GetHeight());
	
	DEBUGCreateRenderGroup(&gRenderGroup, 4096);
	DEBUGCreateBufferGroup(&gRenderBuffer, sizeof(TileVertex), 65536);
	DEBUGAddAttribute(&gRenderBuffer, 0, 2, GL_FLOAT, sizeof(TileVertex), (GLvoid*)offsetof(TileVertex, position));
	DEBUGAddAttribute(&gRenderBuffer, 1, 2, GL_FLOAT, sizeof(TileVertex), (GLvoid*)offsetof(TileVertex, texCoord));
	DEBUGAddAttribute(&gRenderBuffer, 2, 4, GL_FLOAT, sizeof(TileVertex), (GLvoid*)offsetof(TileVertex, color));

    gTerrainShader.shaderProgramID = LoadShaderFromFile("shaders/Terrain2DVertex.glsl","shaders/Terrain2DFragment.glsl");
    gTerrainShader.projectionUniformLocation = GetUniformLocation(gTerrainShader.shaderProgramID, "projection");
    gTerrainShader.isWaterUniformLocation = GetUniformLocation(gTerrainShader.shaderProgramID, "isWater");
    gTerrainShader.waveAngleUniformLocation = GetUniformLocation(gTerrainShader.shaderProgramID, "waveAngle");

	LoadTextureAtlasFromFile(&gTextureAtlas, "textures/foliage/trees.atlas");
	//LoadTextureAtlasFromFile(&gTextureAtlas, ASSET("textures/foliage/trees.atlas"));
	gFoliageAtlasTextureID = CreateTextureFromPixels(gTextureAtlas.width, gTextureAtlas.height, gTextureAtlas.pixels);

	spriteProgramID = LoadShaderFromFile("shaders/Sprite.vert", "shaders/Sprite.frag");
	nullTextureID = CreateTextureFromFile("textures/null.png");

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

	CreateTerrain(&gTerrain, 1024, 1024);
	gPlayerTransform.position.x = gTerrain.widthInTiles * 0.5f;
	gPlayerTransform.position.y = gTerrain.heightInTiles * 0.5f;
	gPlayerTransform.size.x = 0.5f;
	gPlayerTransform.size.y = 0.5f;
	gCameraTransform.position = gPlayerTransform.position + (gPlayerTransform.size * 0.5f);
	gTerrainTextureID = TerrainToTexture(&gTerrain);


	app.Run(MainLoop);
	app.Destroy();


	glDeleteTextures(1, &gFoliageAtlasTextureID);
	FreeTextureAtlas(&gTextureAtlas);


    glDeleteProgram(gTerrainShader.shaderProgramID);
	DestroyTerrain(&gTerrain);
	return 0;
}
#endif