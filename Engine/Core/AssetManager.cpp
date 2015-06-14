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

AssetID AssetManager::LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) {
	AssetID id = registry.data.size();
	registry.data.push_back(nullptr);
	TaskManager::Instance().ScheduleTask<LoadShaderTask>(id, &registry, vertexShaderFile, fragmentShaderFile);
	return id;
}


void LoadFontTask::Run()  {
	FT_Error error;
	FT_Library library;

	//Initalize FT
	error = FT_Init_FreeType(&library);
	if (error) {
		std::cout << "ERROR: Could not init FreeType Library" << std::endl;
		return;
	}
	else
		std::cout << "Initalized Freetype Library" << "\n";

	//Create a freetype face
	FT_Face face;
	error = FT_New_Face(library, filename.c_str(), 0, &face);
	if (error) {
		std::cout << "ERROR: Failed to load font: " << filename << std::endl;
		return;
	}
	else
		std::cout << "Loaded Font: " << filename << "\n";

	FT_Set_Pixel_Sizes(face, 0, fontsize);
	font = new Font();

	font->glyphs.reserve(128);

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
		font->glyphs.push_back(Glyph{
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,
			face->glyph->advance.x >> 6
		});

		//Setup the UVS for the glyph
		Glyph& glyph = font->glyphs[i - 32];
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

	//Generate the GLtexture



	//Update the font with the parameters of our created texture

	font->bitmapWidth = bitmapWidth;
	font->bitmapHeight = bitmapHeight;

	//Clean up
	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

void LoadFontTask::Finalize() {
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

	registry->data[id] = font;
}


AssetManager& AssetManager::Instance() {
	static AssetManager* instance = nullptr;
	if (instance == nullptr)
		instance = new AssetManager();
	return *instance;
}

std::shared_ptr<ImageData> AssetManager::GetImage(const std::string& filename) {
	auto data = std::make_shared<ImageData>();


	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.c_str(), 0);
	if (format == FIF_UNKNOWN) {
		std::cout << "ERROR: Could not read file: " << filename.c_str() << std::endl;
		return nullptr;
	}

	FIBITMAP* image = FreeImage_Load(format, filename.c_str());
	if (!image) {
		std::cout << "ERROR: Image file was found and reconized but it could not be loaded" << std::endl;
		return nullptr;
	}

	FIBITMAP* temp = FreeImage_ConvertTo32Bits(image);
	auto tempType = FreeImage_GetColorType(temp);
	FreeImage_Unload(image);
	image = temp;


	data->width  = FreeImage_GetWidth(image);
	data->height = FreeImage_GetHeight(image);
	data->pixels = new RGBA8[data->width * data->height];
	auto type = FreeImage_GetColorType(image);

	uint8* pixels = FreeImage_GetBits(image);
	for (uint32 i = 0; i < (data->width * data->height); i++) {
		data->pixels[i].r = pixels[(i * 4) + 2];
		data->pixels[i].g = pixels[(i * 4) + 1];
		data->pixels[i].b = pixels[(i * 4) + 0];
		data->pixels[i].a = pixels[(i * 4) + 3];
	}

	return data;
}

AssetID AssetManager::LoadFont(const std::string& filename, uint32 fontSize) {
	AssetID id = registry.data.size();
	registry.data.push_back(nullptr);
	TaskManager::Instance().ScheduleTask<LoadFontTask>(id, &registry, filename, fontSize);
	return id;
}