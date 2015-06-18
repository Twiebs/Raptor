#include "ModelLoader.hpp"

LoadModelTask::LoadModelTask(AssetID id, AssetRegistry* registry, std::string& filename) : 
	id(id), registry(registry), filename(filename) {

}

LoadModelTask::~LoadModelTask() {

}

void LoadModelTask::Run(uint32 threadID) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);

	//The assimp scene contains invalid data
	Model* model = new Model();
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::function<void(aiNode*)> processNode = [&model, &scene, &processNode](aiNode* node) {

			//Determine how many vertices will be included in the mesh
			uint32 numVertices = 0;
			for (uint32 i = 0; i < node->mNumMeshes; i++) {
				numVertices += scene->mMeshes[node->mMeshes[i]]->mNumVertices;
			}	//TODO use better mesh allocation strategy



			for (uint32 i = 0; i < node->mNumMeshes; i++) {
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
					auto loadMaterialTextures = [&material](std::vector<GLTexture*>& textures, aiTextureType type) {
						for (int i = 0; i < material->GetTextureCount(type); i++) {
							aiString texturePath;
							material->GetTexture(type, i, &texturePath);
							//TODOCheck if the texture was allready loaded

							//GLTexture* texture = ResourceManager::Instance().GetGLTexture(model->directory + "/" + texturePath.C_Str());
							//FIXME Model loading is currently broken!
							GLTexture* texture = nullptr;
							texture->type = (type == aiTextureType_DIFFUSE ? TextureType::Diffuse : TextureType::Specular);
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

	}

	result = model;
}

void LoadModelTask::Finalize(uint32 threadID) {
	registry->data[id - 1] = (void*)result;
}