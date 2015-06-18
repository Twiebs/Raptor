#include "AssetManager.hpp"


AssetManager::AssetManager() {
}


AssetManager::~AssetManager() {
}

LoadFontTask::LoadFontTask(AssetID id, AssetRegistry* registry, std::string filename, uint32 fontsize) :
	id(id), 
	registry(registry),
	filename(filename),
	fontsize(fontsize)
{ /*Empty Constructor*/ }

LoadFontTask::~LoadFontTask() {

}


LoadPixmapTask::LoadPixmapTask(AssetID id, AssetRegistry* registry, std::string& filename) : 
	id(id), registry(registry), filename(filename) {
}

LoadPixmapTask::~LoadPixmapTask() {
}

void LoadPixmapTask::Run(uint32 threadID) {
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.c_str(), 0);
	if (format == FIF_UNKNOWN)
		std::cout << "ERROR: Could not read file: " << filename.c_str() << std::endl;

	FIBITMAP* image = FreeImage_Load(format, filename.c_str());
	if (!image)
		std::cout << "ERROR: Image file was found and reconized but it could not be loaded" << std::endl;

	FIBITMAP* temp = FreeImage_ConvertTo32Bits(image);
	FreeImage_Unload(image);
	image = temp;

	pixmap = new Pixmap(FreeImage_GetWidth(image), FreeImage_GetHeight(image), PixmapFormat::RGBA8);
	uint8* pixels = FreeImage_GetBits(image);
	for (uint32 i = 0; i < (pixmap->width * pixmap->height * 4); i += 4) {
		pixmap->data[i + 0] = pixels[i + 2];
		pixmap->data[i + 1] = pixels[i + 1];
		pixmap->data[i + 2] = pixels[i + 0];
		pixmap->data[i + 3] = pixels[i + 3];
	}

	FreeImage_Unload(image);
}

void LoadPixmapTask::Finalize(uint32 threadID) {
	registry->data[id - 1] = pixmap;
}

void LoadFontTask::Run(uint32 threadID)  {
	FT_Error error;
	FT_Library library;

	//Initalize FT
	error = FT_Init_FreeType(&library);
	if (error) {
		std::cout << "ERROR: Could not init FreeType Library" << "\n";
		return;
	}
	else
		std::cout << "Initalized Freetype Library" << "\n";

	//Create a freetype face
	FT_Face face;
	error = FT_New_Face(library, filename.c_str(), 0, &face);
	if (error) {
		std::cout << "ERROR: Failed to load font: " << filename << "\n";
		return;
	}
	else
		std::cout << "Loaded Font: " << filename << "\n";

	FT_Set_Pixel_Sizes(face, 0, fontsize);
	font = new Font();
	//Now things get intresting...
	//Generate a bitmap with the bitmaps of each individual FT glyph bitmap packed into it
	uint32 bitmapGlyphWidth = fontsize;
	uint32 bitmapGlyphHeight = fontsize;
	uint32 bitmapWidthInGlyphs = 12;
	uint32 bitmapHeightInGlyphs = 8;
	uint32 bitmapWidth = MathUtils::NextPowerOfTwo(bitmapWidthInGlyphs  * bitmapGlyphWidth);
	uint32 bitmapHeight = bitmapWidth;
	bitmapFont = new uint8[bitmapWidth * bitmapHeight];
	for (uint32 i = 0; i < bitmapWidth*bitmapHeight; i++) {
		bitmapFont[i] = 0;
	}

	for (uint8 i = 32; i < 127; i++) {
		uint32 glyphIndexY = (i / bitmapWidthInGlyphs);
		uint32 glyphIndexX = i - (glyphIndexY * bitmapWidthInGlyphs);
		uint32 bitmapX = glyphIndexX * bitmapGlyphWidth;
		uint32 bitmapY = glyphIndexY * bitmapGlyphHeight;
		error = FT_Load_Char(face, i, FT_LOAD_RENDER);
		if (error) {
			std::cout << "ERROR: Freetype failed to load glyph";
			continue;
		}

		//Create a new glyph for the font
		Glyph& glyph = font->glyphs[i - 32];
		glyph.width		= face->glyph->bitmap.width;
		glyph.height	= face->glyph->bitmap.rows;
		glyph.bearingX	= face->glyph->bitmap_left;
		glyph.bearingY	= face->glyph->bitmap_top;
		glyph.advance	= face->glyph->advance.x >> 6;
		glyph.uvs[0] = Vector2((float)bitmapX / (float)bitmapWidth, (float)bitmapY / bitmapHeight);
		glyph.uvs[1] = Vector2(((float)bitmapX + (float)glyph.width) / (float)bitmapWidth, (float)bitmapY / (float)bitmapHeight);
		glyph.uvs[2] = Vector2((float)(bitmapX + glyph.width) / (float)bitmapWidth, (float)(bitmapY + glyph.height) / (float)bitmapHeight);
		glyph.uvs[3] = Vector2((float)bitmapX / (float)bitmapWidth, (float)(bitmapY + glyph.height) / (float)bitmapHeight);

		for (int glyphX = 0; glyphX < glyph.width; glyphX++){
			for (int glyphY = 0; glyphY < glyph.height; glyphY++) {
				int bitmapPosX = bitmapX + glyphX;
				int bitmapPosY = bitmapY + glyphY;
				int bitmapIndex = (bitmapPosY * bitmapWidth) + bitmapPosX;
				int glyphBufferIndex = (((glyph.height - 1) - glyphY) * glyph.width) + glyphX;
				bitmapFont[bitmapIndex] = face->glyph->bitmap.buffer[glyphBufferIndex];
			}
		}
	}

	font->bitmapWidth = bitmapWidth;
	font->bitmapHeight = bitmapHeight;

	//Clean up
	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

void LoadFontTask::Finalize(uint32 threadID) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->bitmapWidth, font->bitmapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, &bitmapFont[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	font->textureID = textureID;

	registry->data[id - 1] = font;
}


AssetID AssetManager::LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {
	auto id = GetNextAvaiableID();
	engine->taskManager->ScheduleTask<LoadShaderTask>(id, &registry, vertexShaderFile, fragmentShaderFile);
	return id;
}

AssetID AssetManager::LoadPixmap(const std::string& filename) {
	auto id = GetNextAvaiableID();
	engine->taskManager->ScheduleTask<LoadPixmapTask>(id, &registry, filename);
	return id;
}

AssetID AssetManager::LoadModel(const std::string& filename) {
	auto id = GetNextAvaiableID();
	engine->taskManager->ScheduleTask<LoadModelTask>(id, &registry, filename);
	return id;
}

AssetID AssetManager::LoadFont(const std::string& filename, uint32 fontSize) {
	auto id = GetNextAvaiableID();
	engine->taskManager->ScheduleTask<LoadFontTask>(id, &registry, filename, fontSize);
	return id;
}

AssetID AssetManager::GetNextAvaiableID() {
	AssetID id = registry.data.size() + 1;
	registry.data.push_back(nullptr);
	return id;
}