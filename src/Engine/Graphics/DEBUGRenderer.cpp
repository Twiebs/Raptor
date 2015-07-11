#include "DEBUGRenderer.hpp"

void DEBUGFlushGroup(DEBUGRenderGroup* group) {
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, group->currentTextureID);

	glBindVertexArray(group->vertexArrayID);
	glDrawElements(GL_TRIANGLES, (group->currentVertexCount / 4) * 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	group->currentVertexCount = 0;
	group->drawCalls++;
}

void DEBUGPushVertices(DEBUGRenderGroup* group, Vert* vertices, uint32 count) {
	if (group->currentVertexCount + count > group->maxVertexCount)
		DEBUGFlushGroup(group);

	glBindBuffer(GL_ARRAY_BUFFER, group->vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vert) * group->currentVertexCount, sizeof(Vert) * count, (GLvoid*)vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	group->currentVertexCount += count;
}

void DEBUGDrawTexture(DEBUGRenderGroup* group, GLuint textureID, float x, float y, float width, float height, Color color) {
	if (group->currentTextureID != textureID) {
		DEBUGFlushGroup(group);
		group->currentTextureID = textureID;
	}

	Vert verts[4];
	verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), color };
	verts[1] = Vert{ Vector2(x + width, y), Vector2(1.0f, 0.0f), color };
	verts[2] = Vert{ Vector2(x + width, y + height), Vector2(1.0f, 1.0f), color };
	verts[3] = Vert{ Vector2(x, y + height), Vector2(0.0f, 1.0f), color };
	DEBUGPushVertices(group, verts, 4);
}

 void DEBUGDrawCircle(DEBUGRenderGroup* group, float32 centerX, float32 centerY, float32 radius, uint32 numSegments) {
	float32 theta = (2.0f * PI32) / float32(numSegments);
	float32 c = cosf(theta);
	float32 s = sinf(theta);
	float32 x = radius;
	float32 y = 0;

	float32 lastX;
	std::vector<Vert> verts(numSegments);
	for (uint32 i = 0; i < numSegments; i++) {
		verts.emplace_back(Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), Color() });

		lastX = x;
		x = (c * x) - (s * y);
		y = (s * lastX) + (c * y);
	}
}

void DEBUGFillRect(DEBUGRenderGroup* group, float32 x, float32 y, float32 width, float32 height, Color color) {
	if (group->currentTextureID != 0) {
		DEBUGFlushGroup(group);
		group->currentTextureID = 0;
	}

	Vert verts[4];
	verts[0] = Vert{ Vector2(x, y), Vector2(0.0f, 0.0f), color };
	verts[1] = Vert{ Vector2(x + width, y), Vector2(1.0f, 0.0f), color };
	verts[2] = Vert{ Vector2(x + width, y + height), Vector2(1.0f, 1.0f), color };
	verts[3] = Vert{ Vector2(x, y + height), Vector2(0.0f, 1.0f), color };
	DEBUGPushVertices(group, verts, 4);
}

void DEBUGCreateRenderGroup(DEBUGRenderGroup* group, uint32 maxVertexCount) {
	glGenVertexArrays(1, &group->vertexArrayID);
	glBindVertexArray(group->vertexArrayID);

	glGenBuffers(1, &group->vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, group->vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vert) * maxVertexCount, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, position));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, uv));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, color));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//TODO add facilities to obtain a block of temporary memory
	//TODO look into mapping the address of the element buffer into our address space and
	//Manually pushing the indices to it!
	uint32 indexCount = (maxVertexCount / 4) * 6;
	uint32* indices = new uint32[indexCount];
	for (uint32 i = 0, v = 0; i < indexCount; i += 6, v += 4) {
		indices[i + 0] = v + 0;
		indices[i + 1] = v + 3;
		indices[i + 2] = v + 2;
		indices[i + 3] = v + 0;
		indices[i + 4] = v + 2;
		indices[i + 5] = v + 1;
	}

	glGenBuffers(1, &group->elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group->elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * indexCount, indices, GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
	delete[] indices;

	group->currentVertexCount = 0;
	group->maxVertexCount = maxVertexCount;
}
