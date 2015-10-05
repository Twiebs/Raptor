#ifndef RAPTOR_PHYSICS2D_HPP
#define RAPTOR_PHYSICS2D_HPP

#include <Box2D/Box2D.h>

struct Box2DRenderer : public b2Draw {
    virtual void DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color);
    virtual void DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color);
    virtual void DrawCircle(const b2Vec2 &center, float32 radius, const b2Color &color);
    virtual void DrawSolidCircle(const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &color);
    virtual void DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color);
    virtual void DrawTransform(const b2Transform &xf);
    Box2DRenderer();
    virtual ~Box2DRenderer();
};

#endif // RAPTOR_PHYSICS2D_HPP
