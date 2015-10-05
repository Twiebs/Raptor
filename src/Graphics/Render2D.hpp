#pragma once

#include <Graphics/Color.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Vertex2D.hpp>
#include <Math/Matrix4.hpp>

struct Vertex2D {
	Vector2 position;
	Vector2 uv;
	Color color;
};

struct IndexedDrawBuffer {
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint elementBufferID;

    U32 currentVertexCount;
    U32 maxVertexCount;
    Vertex2D* vertices;

    U32 currentIndexCount;
    U32 maxIndexCount;
    U32* indices;
};

void InitDrawBuffer (IndexedDrawBuffer* buffer, U32 maxVertexCount);
void FreeDrawBuffer (IndexedDrawBuffer* buffer);
void Bind (IndexedDrawBuffer* buffer);
void Draw (IndexedDrawBuffer* buffer, GLenum drawMode = GL_TRIANGLES);

void PushRect (IndexedDrawBuffer* buffer, Vector2* vertices, U32 count, const Color& color = Color::WHITE);
void PushRect (IndexedDrawBuffer* buffer, F32 x, F32 h, F32 width, F32 height, const Color& color = Color::WHITE);
//void PushTexture (IndexedDrawBuffer* buffer, GLuint textureID, float x, float y, float width, float height, Color color = Color());



//void PushVertices (IndexedDrawBuffer* buffer, Vertex2D* vertices, U32 count);
//void PushVertices (IndexedDrawBuffer* buffer, Vector2* vertices, U32 count, const Color& color);

struct Camera2D {
    Vector2 position;
    Matrix4 projection;
    Matrix4 view;
    float viewportWidth, viewportHeight;
    Camera2D(float viewportWidth, float viewportHeight);
    void update();
    Matrix4 projectionView();
};



struct Sprite {
    float x, y;
    float width, height;
    GLuint textureID;
};

class SpriteBatch {
public:
    SpriteBatch();
    ~SpriteBatch();

    void begin(const Matrix4& projectionView);
    void end();
    void add(Sprite* sprite);
    void add(float x, float y, float width, float height, U32 textureID);

private:
    IndexedDrawBuffer buffer;
    GLuint shader;
    GLint projectionViewLoc;
};
