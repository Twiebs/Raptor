#include "Render2D.hpp"
#include <Graphics/GLSLProgram.hpp>

// TODO Consider returning an int code when items are pushed into the buffer
// so that you can determine what to do if it fails instead of defaulting to
// drawing the buffer

Camera2D::Camera2D(float viewportWidth, float viewportHeight)
    : viewportWidth(viewportWidth), viewportHeight(viewportHeight) {
    projection = Matrix4::Ortho(0.0f, viewportWidth, 0.0f, viewportHeight, 0.1f, 100.0f, 1.0f);
}

void Camera2D::update() {
    view = Matrix4::Translate(-position.x + (viewportWidth*0.5f), -position.y + (viewportHeight*0.5f), 0.0f);
}

Matrix4 Camera2D::projectionView() {
    return projection * view;
}

void InitDrawBuffer (IndexedDrawBuffer* buffer, U32 maxVertexCount) {
    buffer->currentVertexCount = 0;
    buffer->maxVertexCount = maxVertexCount;
    buffer->maxIndexCount = (maxVertexCount / 4) * 6;
    size_t vertexMemorySize = sizeof(Vertex2D) * maxVertexCount;
    size_t indexMemorySize = sizeof(U32) * buffer->maxIndexCount;
    buffer->vertices = (Vertex2D*)malloc(vertexMemorySize + indexMemorySize);
    buffer->indices = (U32*)(buffer->vertices + maxVertexCount);

    glGenVertexArrays(1, &buffer->vertexArrayID);
    glBindVertexArray(buffer->vertexArrayID);

    glGenBuffers(1, &buffer->vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * maxVertexCount, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, uv));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, color));

    glGenBuffers(1, &buffer->elementBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->elementBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(U32) * buffer->maxIndexCount, nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void FreeDrawBuffer (IndexedDrawBuffer* buffer) {
    glDeleteBuffers(1, &buffer->vertexBufferID);
    glDeleteBuffers(1, &buffer->elementBufferID);
    glDeleteVertexArrays(1, &buffer->vertexArrayID);
    free(buffer->vertices);
    buffer->maxVertexCount = 0;
}

void Bind(IndexedDrawBuffer* buffer) {
    assert(buffer->currentVertexCount == 0 && "Buffer was not drawn since last bind");
}

void PushRect (IndexedDrawBuffer* buffer, Vector2* vertices, U32 count, const Color& color) {
    if (buffer->currentVertexCount + count > buffer->maxVertexCount) {
        Draw(buffer);
    }

    for (U32 i = 0; i < count; i++) {
        Vector2* position = &vertices[i];
        Vertex2D* vertex = &buffer->vertices[i + buffer->currentVertexCount];
        vertex->position.x = position->x;
        vertex->position.y = position->y;
		vertex->color = color;
    }

    buffer->indices[buffer->currentIndexCount + 0] = buffer->currentVertexCount + 0;
    buffer->indices[buffer->currentIndexCount + 1] = buffer->currentVertexCount + 1;
    buffer->indices[buffer->currentIndexCount + 2] = buffer->currentVertexCount + 2;
    buffer->indices[buffer->currentIndexCount + 3] = buffer->currentVertexCount + 2;
    buffer->indices[buffer->currentIndexCount + 4] = buffer->currentVertexCount + 3;
    buffer->indices[buffer->currentIndexCount + 5] = buffer->currentVertexCount + 0;
    buffer->currentVertexCount += count;
    buffer->currentIndexCount += 6;
}

void PushRect (IndexedDrawBuffer* buffer, F32 x, F32 y, F32 width, F32 height, const Color& color) {
    if (buffer->currentVertexCount + 4 > buffer->maxVertexCount ||
        buffer->currentIndexCount + 6 > buffer->maxIndexCount) {
        Draw(buffer);
    }

    buffer->vertices[0 + buffer->currentVertexCount].position = Vector2(x, y);
    buffer->vertices[1 + buffer->currentVertexCount].position = Vector2(x + width, y);
    buffer->vertices[2 + buffer->currentVertexCount].position = Vector2(x + width, y + height);
    buffer->vertices[3 + buffer->currentVertexCount].position = Vector2(x, y + height);
	buffer->vertices[0 + buffer->currentVertexCount].uv = Vector2(0.0f, 0.0f);
	buffer->vertices[1 + buffer->currentVertexCount].uv = Vector2(1.0f, 0.0f);
	buffer->vertices[2 + buffer->currentVertexCount].uv = Vector2(1.0f, 1.0f);
	buffer->vertices[3 + buffer->currentVertexCount].uv = Vector2(0.0f, 1.0f);
	buffer->vertices[0 + buffer->currentVertexCount].color = color;
	buffer->vertices[1 + buffer->currentVertexCount].color = color;
	buffer->vertices[2 + buffer->currentVertexCount].color = color;
	buffer->vertices[3 + buffer->currentVertexCount].color = color;

    buffer->indices[buffer->currentIndexCount + 0] = buffer->currentVertexCount + 0;
    buffer->indices[buffer->currentIndexCount + 1] = buffer->currentVertexCount + 1;
    buffer->indices[buffer->currentIndexCount + 2] = buffer->currentVertexCount + 2;
    buffer->indices[buffer->currentIndexCount + 3] = buffer->currentVertexCount + 2;
    buffer->indices[buffer->currentIndexCount + 4] = buffer->currentVertexCount + 3;
    buffer->indices[buffer->currentIndexCount + 5] = buffer->currentVertexCount + 0;

    buffer->currentVertexCount += 4;
    buffer->currentIndexCount += 6;
}

//void PushTexture (IndexedDrawBuffer* buffer, GLuint textureID, float x, float y, float width, float height, Color color) {
//    Vertex2D verts[4];
//    verts[0] = Vertex2D{ Vector2(x, y), Vector2(0.0f, 0.0f), color };
//    verts[1] = Vertex2D{ Vector2(x + width, y), Vector2(1.0f, 0.0f), color };
//    verts[2] = Vertex2D{ Vector2(x + width, y + height), Vector2(1.0f, 1.0f), color };
//    verts[3] = Vertex2D{ Vector2(x, y + height), Vector2(0.0f, 1.0f), color };
//    PushVertices(buffer, verts, 4);
//}

void Draw (IndexedDrawBuffer* buffer, GLenum drawMode) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vertexBufferID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex2D) * buffer->currentVertexCount, (GLvoid*)buffer->vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->elementBufferID);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(U32) * buffer->currentIndexCount, (GLvoid*)buffer->indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(buffer->vertexArrayID);
    glDrawElements(drawMode, buffer->currentIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    buffer->currentVertexCount = 0;
    buffer->currentIndexCount = 0;
}

SpriteBatch::SpriteBatch() {
    InitDrawBuffer(&buffer, 1 << 16);
    std::string vertexSource = R"(
	#version 100
	attribute vec2 position;
	attribute vec2 uv;
	attribute vec4 color;

	varying vec2 fragUV;
	varying vec4 fragColor;

	uniform mat4 projectionView;
	void main() {
		fragUV = uv;
		fragColor = color;
		gl_Position = projectionView * vec4(position, 0.0, 1.0);
	}
    )";

    std::string fragmentSource = R"(
	#version 100
	precision mediump float;

	varying vec2 fragUV;
	varying vec4 fragColor;

	uniform sampler2D sampler;

	void main() {
		gl_FragColor = fragColor;
		// gl_FragColor = vec4(1.0, 0.6, 0.2, 1.0);
	}
    )";

    shader = CreateShaderFromSource(vertexSource.c_str(), fragmentSource.c_str());
    projectionViewLoc = GetUniformLocation(shader, "projectionView");
}

SpriteBatch::~SpriteBatch() {
    FreeDrawBuffer(&buffer);
    glDeleteProgram(shader);
}

void SpriteBatch::begin(const Matrix4& projectionView) {
    Bind(&buffer);
    glUseProgram(shader);
    glUniformMatrix4fv(projectionViewLoc, 1, GL_FALSE, &projectionView[0][0]);
}

void SpriteBatch::end() {
    Draw(&buffer);
}

void SpriteBatch::add(float x, float y, float width, float height, U32 textureID) {
    //PushTexture(&buffer, textureID, x, y, width, height, Color());
}

void SpriteBatch::add(Sprite *sprite) {
    //PushTexture(&buffer, sprite->textureID, sprite->x, sprite->y, sprite->width, sprite->height);
}