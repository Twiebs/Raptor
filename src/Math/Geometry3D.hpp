#ifndef RAPTOR_GEOMETRY3D_H
#define RAPTOR_GEOMETRY3D_H

#include <Math/Vector3.hpp>
#include <Math/Vector2.hpp>

struct Cuboid {
	union 
	{
		V3 position;
		struct { float x, y, z; };
	};

	union  
	{
		V3 size;
		struct { float w, h, l;};
	};
	
	Cuboid() { }
};

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
