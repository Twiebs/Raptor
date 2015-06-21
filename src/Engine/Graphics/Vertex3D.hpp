#pragma once
#include <Math/Vector2.hpp>
#include <Math/Vector3.hpp>

struct Vertex3D {
	Vector3 position;
	Vector3 normal;
	Vector2 uv;

	Vertex3D() {

	}

	Vertex3D(Vector3 position) {
		this->position = position;
	}

	Vertex3D(Vector3 position, Vector2 uv) {
		this->position = position;

		this->uv = uv;
	}

	Vertex3D(Vector3 position, Vector3 normal) {
		this->position = position;
		this->normal = normal;
	}

	Vertex3D(Vector3 position, Vector3 normal, Vector2 uv) {
		this->position = position;
		this->normal = normal;
		this->uv = uv;
	}
};
