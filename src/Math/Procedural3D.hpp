#ifndef RAPTOR_PROCEDURAL3D_HPP
#define RAPTOR_PROCEDURAL3D_HPP

#include <functional>

#include <Graphics/Mesh.hpp>

void GetPlaneMeshMemorySize(U32 meshResolution, U32* vertexCount, U32* indexCount);
void GeneratePlaneMesh(MeshData* meshData, float x, float y, float w, float h, int cellCount, int cellsPerUV, std::function<float(float, float)> heightFunction = nullptr);
void CreatePlaneMesh (MeshData* meshData, float x, float y, float w, float h, int cellCount, int cellsPerUV, std::function<float(float,float)> heightFunction = nullptr);


#endif //RAPTOR_PROCEDURAL3D_HPP
