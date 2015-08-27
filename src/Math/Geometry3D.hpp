#ifndef RAPTOR_GEOMETRY3D_H
#define RAPTOR_GEOMETRY3D_H

#include <Math/Vector3.hpp>
#include <Math/Vector2.hpp>

Vector3 PointToQuadrilateralizedSphere(const Vector3& point);

U32 GetVertexCountForQuad(int subdivisionLevel);
U32 GetIndexCountForQuad(int subdivisionLevel);

void BuildCube(int subdivisionLevel, Vector3* positions, U32* indices);
void BuildFrontQuad(int subdivisionLevel, Vector3* positions, U32* indices);
void BuildBackQuad(int subdivisionLevel, Vector3* positions, U32* indices);
void BuildTopQuad(int subdivisionLevel, Vector3* positions, U32* indices);
void BuildBottomQuad(int subdivisionLevel, Vector3* positions, U32* indices);
void BuildLeftQuad(int subdivisionLevel, Vector3* positions, U32* indices);
void BuildRightQuad(int subdivisionLevel, Vector3* positions, U32* indices);

#endif //RAPTOR_GEOMETRY3D_H
