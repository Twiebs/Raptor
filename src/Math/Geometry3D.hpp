#ifndef RAPTOR_GEOMETRY3D_H
#define RAPTOR_GEOMETRY3D_H

#include <Math/Vector3.hpp>
#include <Math/Vector2.hpp>

static const V3 CUBE_VERTICES[24] = {
	// Front face
	V3(-1.0, -1.0,  1.0),
	V3(1.0,  -1.0,  1.0),
	V3(1.0,   1.0,  1.0),
	V3(-1.0,  1.0,  1.0),

	// Back face
	V3(-1.0, -1.0, -1.0),
	V3(-1.0,  1.0, -1.0),
	V3( 1.0,  1.0, -1.0),
	V3( 1.0, -1.0, -1.0),

	// Top face
	V3(-1.0,  1.0, -1.0),
	V3(-1.0,  1.0,  1.0),
	V3( 1.0,  1.0,  1.0),
	V3( 1.0,  1.0, -1.0),

	// Bottom face
	V3(-1.0, -1.0, -1.0),
	V3( 1.0, -1.0, -1.0),
	V3( 1.0, -1.0,  1.0),
	V3(-1.0, -1.0,  1.0),

	// Right face
	V3(1.0, -1.0, -1.0),
	V3(1.0,  1.0, -1.0),
	V3(1.0,  1.0,  1.0),
	V3(1.0, -1.0,  1.0),

	// Left face
	V3(-1.0, -1.0, -1.0),
	V3(-1.0, -1.0,  1.0),
	V3(-1.0,  1.0,  1.0),
	V3(-1.0,  1.0, -1.0)
};

static const U32 CUBE_INDICES[36] = {
	0,  1,  2,      0,  2,  3,    // front
	4,  5,  6,      4,  6,  7,    // back
	8,  9,  10,     8,  10, 11,   // top
	12, 13, 14,     12, 14, 15,   // bottom
	16, 17, 18,     16, 18, 19,   // right
	20, 21, 22,     20, 22, 23    // left
};


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

	V3 Center();
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
