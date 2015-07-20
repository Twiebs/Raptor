#include <Core/Application.hpp>
#include <Math/Vector3.hpp>
#include <Math/Vector2.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct Vertex3D {
    Vector3 position;
    Vector3 normal;
    Vector2 texCoord;
};

struct Mesh {
    U32 vertexCount;
    U32 indexCount;
    U8* memblock;
    Vertex3D* vertices;
    U32* indices;
};

void AllocateMesh(Mesh* mesh, U32 vertexCount , U32 indexCount) {
    mesh->vertexCount = vertexCount;
    mesh->indexCount = indexCount;
    auto verticesMemorySize = sizeof(Vertex3D) * vertexCount;
    auto indiciesMemorySize = sizeof(U32) * indexCount;
    mesh->memblock = new U8[verticesMemorySize * indiciesMemorySize];
    mesh->vertices = (Vertex3D*)mesh->memblock;
    mesh->indices = (U32*)mesh->memblock + verticesMemorySize;
}

void FreeMesh(Mesh* mesh) {
    mesh->vertexCount = 0;
    mesh->indexCount = 0;
    delete[] mesh->memblock;
    mesh->vertices = nullptr;
    mesh->indices = nullptr;
}

void LoadMeshFromFile(Mesh* mesh, const char* filename) {
    // When loading meshes from file the mesh must have allready been allocated!
    assert(mesh->vertexCount == 0);
    assert(mesh->indexCount == 0);
    assert(mesh->memblock == nullptr);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_ERROR("Could not load scene when attempting to load mesh" << filename);
        return;
    }

    //Determine how many vertices will be included in the mesh
    auto node = scene->mRootNode;
    U32 vertexCount = 0;
    U32 indexCount = 0;
    for (U32 i = 0; i < node->mNumMeshes; i++) {
        vertexCount += scene->mMeshes[node->mMeshes[i]]->mNumVertices;
        for(U32 j = 0; j < scene->mMeshes[node->mMeshes[i]]->mNumFaces; j++) {
            indexCount += scene->mMeshes[node->mMeshes[i]]->mFaces[j].mNumIndices;
        }
    }

    AllocateMesh(mesh, vertexCount, indexCount);

    U32 vertexIndex = 0;
    U32 indexIndex = 0;
    for (uint32 imesh = 0; imesh < node->mNumMeshes; imesh++) {
        aiMesh *assimpMesh = scene->mMeshes[node->mMeshes[imesh]];
        for (U32 i = 0; i < assimpMesh->mNumVertices; i++) {
            mesh->vertices[vertexIndex].position.x = assimpMesh->mVertices[i].x;
            mesh->vertices[vertexIndex].position.y = assimpMesh->mVertices[i].y;
            mesh->vertices[vertexIndex].position.z = assimpMesh->mVertices[i].z;
            mesh->vertices[vertexIndex].normal.x = assimpMesh->mNormals[i].x;
            mesh->vertices[vertexIndex].normal.y = assimpMesh->mNormals[i].y;
            mesh->vertices[vertexIndex].normal.z = assimpMesh->mNormals[i].z;

            // NOTE What the fuck is this?
            if (assimpMesh->mTextureCoords[0]) {
                mesh->vertices[vertexIndex].texCoord.x = assimpMesh->mTextureCoords[0][i].x;
                mesh->vertices[vertexIndex].texCoord.y = assimpMesh->mTextureCoords[0][i].y;
            } else {
                LOG_ERROR("Mesh does not have textureCoordiantes!");
            }
        }
        vertexIndex++;

        for (uint32 i = 0; i < assimpMesh->mNumFaces; i++) {
            aiFace face = assimpMesh->mFaces[i];
            for (int j = 0; j < face.mNumIndices; j++) {
                mesh->indices[indexIndex] = face.mIndices[j];
                indexIndex++;
            }
        }
    }

}


Mesh mesh;

void MainLoop(Application* app) {

}

#if 1
int main () {
    Application app("Raptor Mesh Viewer", 1280, 720, false);
    LoadMeshFromFile(&mesh, ASSET("models/nanosuit/nanosuit.mtl"));
    app.Run(MainLoop);
}
#endif