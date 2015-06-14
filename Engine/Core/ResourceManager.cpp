#include "ResourceManager.h"

#include<Engine\Engine.h>
#include<functional>

ResourceManager::ResourceManager() {
}


ResourceManager::~ResourceManager() {

}

ResourceManager& ResourceManager::Instance() {
	static ResourceManager* instance = nullptr;
	if (instance == nullptr) {
		instance = new ResourceManager();
	}
	return *instance;
}

Font* ResourceManager::LoadFont(const std::string& filename, uint32 fontSize) {
	auto& font = fonts[filename];

	if (font == nullptr) {
		FT_Error error;
		FT_Library library;
		
		//Initalize FT
		error = FT_Init_FreeType(&library);
		if (error)
			std::cout << "ERROR: Could not init FreeType Library" << std::endl;
		else
			std::cout << "Initalized Freetype Library" << "\n";

		//Create a freetype face
		FT_Face face;
		error = FT_New_Face(library, filename.c_str(), 0, &face);
		if (error)
			std::cout << "ERROR: Failed to load font: " << filename << std::endl;
		else
			std::cout << "Loaded Font: " << filename << "\n";

		FT_Set_Pixel_Sizes(face, 0, fontSize);
		Font* font = new Font();
		fonts[filename] = font;
		font->glyphs.reserve(128);

		//Now things get intresting...
		//Generate a bitmap with the bitmaps of each individual FT glyph bitmap packed into it
		uint32 bitmapGlyphWidth  = fontSize;
		uint32 bitmapGlyphHeight = fontSize;
		uint32 bitmapWidthInGlyphs = 12;
		uint32 bitmapHeightInGlyphs = 8;
		uint32 bitmapWidth  = MathUtils::NextPowerOfTwo(bitmapWidthInGlyphs  * bitmapGlyphWidth);
		uint32 bitmapHeight = bitmapWidth;
		uint8* bitmapFont	= new uint8[bitmapWidth * bitmapHeight];
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
					int bitmapPosX = bitmapX  + glyphX;
					int bitmapPosY = bitmapY  + glyphY;
					int bitmapIndex = (bitmapPosY * bitmapWidth) + bitmapPosX;
					int glyphBufferIndex = (((glyph.height - 1) - glyphY) * glyph.width) + glyphX;
					bitmapFont[bitmapIndex] = face->glyph->bitmap.buffer[glyphBufferIndex];
				}
			}
		}

		//Generate the GLtexture
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmapWidth, bitmapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, &bitmapFont[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Update the font with the parameters of our created texture
		font->textureID = textureID;
		font->bitmapWidth = bitmapWidth;
		font->bitmapHeight = bitmapHeight;

		//Clean up
		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}
	return font;
}


std::unique_ptr<Model> ResourceManager::LoadModel(std::string filename) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);

	//The assimp scene contains invalid data
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR: ResourceManager failed to load:" << filename << std::endl;
		return nullptr;
	}
	
	//Inialize a new model instance
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->directory = filename.substr(0, filename.find_last_of('/'));
	std::function<void(aiNode*)> processNode = [&model, &scene, &processNode](aiNode* node) {
		for (int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			std::vector<Vertex3D> verticies;
			std::vector<GLuint> indices;
			std::vector<GLTexture*> textures;

			//Parse the vertex from assimp into in-engine vertex
			for (int i = 0; i < mesh->mNumVertices; i++) {
				aiVector3D vertPos = mesh->mVertices[i];
				aiVector3D norm = mesh->mNormals[i];

				Vector3 position(vertPos.x, vertPos.y, vertPos.z);
				Vector3 normal(norm.x, norm.y, norm.z);
				Vertex3D vertex(position, normal);
				
				//Check for texture coords
				if (mesh->mTextureCoords[0]) {
					vertex.uv.x = mesh->mTextureCoords[0][i].x;
					vertex.uv.y = mesh->mTextureCoords[0][i].y;
				}
				else {
					std::cout << "No Corords!" << std::endl;
				}
				verticies.push_back(vertex);
			}

			//Convert Assimp indicies into in-engine indicies
			for (int i = 0; i < mesh->mNumFaces; i++) {
				//TODO why are we copping the face??
				aiFace face = mesh->mFaces[i];
				for (int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			//Convert assimp materials into in-engine materials
			if (mesh->mMaterialIndex >= 0) {
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				std::vector<GLTexture*> diffuseMaps;
				std::vector<GLTexture*> specularMaps;

				//Adds all textures of the given type from the material into a texture array 
				auto loadMaterialTextures = [&model, &material](std::vector<GLTexture*>& textures, aiTextureType type) {
					for (int i = 0; i < material->GetTextureCount(type); i++) {
						aiString texturePath;
						material->GetTexture(type, i, &texturePath);
						//TODOCheck if the texture was allready loaded
						
						//GLTexture* texture = ResourceManager::Instance().GetGLTexture(model->directory + "/" + texturePath.C_Str());
						//FIXME Model loading is currently broken!
						GLTexture* texture = nullptr;
						texture->textureType = (type == aiTextureType_DIFFUSE ? "textureDiffuse" : "textureSpecular");
						textures.push_back(texture);
					}
				};

				//Load in the textures from the material via the LoadMaterialTexture Lambda
				loadMaterialTextures(diffuseMaps, aiTextureType_DIFFUSE);
				loadMaterialTextures(specularMaps, aiTextureType_SPECULAR);
				//Add the seperate texture type arrays to the main texture vector
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			}

			model->meshes.push_back(std::make_unique<Mesh>(verticies, indices, textures));
			auto& raptormesh = model->meshes[model->meshes.size() - 1];
			raptormesh->Setup();
		}

		//Recursivly process all of this nodes children
		for (int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i]);
		}
	};

	//Processes the root node of the scene
	//Inside the lambda the next child node is processed
	processNode(scene->mRootNode);

	//The model processing has finished
	return model;
}