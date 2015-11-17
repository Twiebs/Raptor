#pragma once

#include <Math/Vector3.hpp>
#include <Math/Matrix4.hpp>

struct Camera {
	float viewportWidth, viewportHeight;
	float aspectRatio;
	float yaw = -90, pitch = 0;
	float nearClip = 0.1f, farClip = 50.0f;
	float fov;

	Vector3 position;
	Vector3 front;
	Matrix4 view;
	Matrix4 projection;
	Camera(float viewportWidth, float viewportHeight, float fov = 45.0f, float nearClip = 0.1f, float farClip = 50.0f);
};


void UpdateCamera(Camera* camera);
void FPSCameraControlUpdate(Camera* camera);
