#include "PhysicsSystem2D.hpp"

PhysicsRenderer2D::PhysicsRenderer2D() {
    const char* vertexShaderSource =
        "#version 330\n"
        "layout (location = 0) in vec2 position;"
        "layout (location = 1) in vec2 uv;"
        "layout (location = 2) in vec4 color;"
        "out vec2 fragUV;"
        "out vec4 fragColor;"
		"uniform mat4 projection;"
        "void main() {"
        "	fragUV = uv;"
        "	fragColor = color;"
        "	gl_Position = projection * vec4(position.xy, 0.0, 1.0);"
        "};";

    const char* fragmentShaderSource =
        "#version 330\n"
        "in vec2 fragUV;"
        "in vec4 fragColor;"
        "out vec4 outColor;"
        "uniform sampler2D sampler;"
        "void main() {"
        "	outColor = fragColor * texture(sampler, fragUV);"
        "}";

	DEBUGInitGroup(&renderGroup, 4096, vertexShaderSource, fragmentShaderSource);
}

PhysicsRenderer2D::~PhysicsRenderer2D() {
	DEBUGDisposeGroup(&renderGroup);
}

void PhysicsRenderer2D::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {

}
void PhysicsRenderer2D::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {

}
void PhysicsRenderer2D::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {

}
void PhysicsRenderer2D::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {

}
void PhysicsRenderer2D::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {

}
void PhysicsRenderer2D::DrawTransform(const b2Transform& xf) {
	DEBUGPushRect(xf.p.x, xf.p.y, 0.1f, 0.1f, &renderGroup);
}



PhysicsSystem2D::~PhysicsSystem2D() {

}
bool PhysicsSystem2D::Startup(ECSManager* manager) {
	world = new b2World(b2Vec2_zero);
	return true;
}

bool PhysicsSystem2D::Shutdown(ECSManager* manager) {
	delete world;
	return true;
}

void PhysicsSystem2D::Update(float64 deltaTime) {
	world->Step(PHYSICS_2D_TIME_STEP, PHYSICS_2D_VELOCITY_ITERATIONS, PHYSICS_2D_POSITION_ITERATIONS);
}
