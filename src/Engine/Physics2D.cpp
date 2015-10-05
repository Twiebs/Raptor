#include "Physics2D.hpp"
#include <Engine/GFX2D.hpp>
#include <GL/glew.h>

Box2DRenderer::Box2DRenderer() {
    //InitDrawBuffer(&buffer, 1 << 16);
    AppendFlags(e_shapeBit);
    //AppendFlags(e_jointBit);
    AppendFlags(e_aabbBit);
    AppendFlags( e_pairBit);
    AppendFlags(e_centerOfMassBit);
}

Box2DRenderer::~Box2DRenderer() {
    //FreeDrawBuffer(&buffer);
}

void Box2DRenderer::DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) {
	assert(vertexCount == 4);
	GFX2D::FillRect(vertices[0].x, vertices[0].y, vertices[2].x - vertices[0].x, vertices[2].y - vertices[0].y, Color(color.r, color.g, color.b, color.a));
	//glColor4f(color.r, color.g, color.b,1);
	//glVertexPointer(2, GL_FLOAT, 0, vertices);
	//glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
}

void Box2DRenderer::DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) {
//	assert(vertexCount == 4);
//	GFX2D::FillRect(vertices[0].x, vertices[0].y, vertices[2].x - vertices[0].y, vertices[2].y - vertices[0].y, Color(color.r, color.g, color.b, color.a));

	//set up vertex array
	GLfloat glverts[16]; //allow for polygons up to 8 vertices
	glVertexPointer(2, GL_FLOAT, 0, glverts); //tell OpenGL where to find vertices
	glEnableClientState(GL_VERTEX_ARRAY); //use vertices in subsequent calls to glDrawArrays

	//fill in vertex positions as directed by Box2D
	for (int i = 0; i < vertexCount; i++) {
		glverts[i*2]   = vertices[i].x;
		glverts[i*2+1] = vertices[i].y;
	}

	//draw solid area
	glColor4f( color.r, color.g, color.b, 1);
	glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

	//draw lines
	glLineWidth(3); //fat lines
	glColor4f( 1, 0, 1, 1 ); //purple
	glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
}

void Box2DRenderer::DrawCircle(const b2Vec2 &center, float32 radius, const b2Color &color) {

}

void Box2DRenderer::DrawSolidCircle(const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &color) {

}

void Box2DRenderer::DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color) {

}

void Box2DRenderer::DrawTransform(const b2Transform &xf) {

}