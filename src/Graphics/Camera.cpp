#include "Camera.hpp"
#include <Core/Platform.h>

Camera::Camera (float viewportWidth, float viewportHeight, float fov, float nearClip, float farClip)
	: viewportWidth(viewportWidth),
	viewportHeight(viewportHeight),
	fov(fov),
	nearClip(nearClip),
	farClip(farClip) 
{
	aspectRatio = viewportWidth / viewportHeight;
}

void UpdateCamera (Camera* camera) 
{
	camera->front.x = cos(RADIANS(camera->yaw)) * cos(RADIANS(camera->pitch));
	camera->front.y = sin(RADIANS(camera->pitch));
	camera->front.z = sin(RADIANS(camera->yaw)) * cos(RADIANS(camera->pitch));
	camera->front.Normalize();
	camera->view = Matrix4::LookAt(camera->position, camera->position + camera->front, Vector3(0.0f, 1.0f, 0.0f));
	camera->projection = Matrix4::Perspective(camera->fov, camera->aspectRatio, camera->nearClip, camera->farClip);
}


void FPSCameraControlUpdate(Camera* camera) 
{
	float deltaTime = PlatformGetDeltaTime();

	int dx, dy;
	PlatformGetCursorDelta(&dx, &dy);

	camera->yaw += dx;
	camera->pitch -= dy;

	static const float MOVEMENT_SPEED = 1.788f;
	static const float SPRINT_MULTIPLIER = 3.0f;
	float speed = MOVEMENT_SPEED;

	if (PlatformGetKey(KEY_LSHIFT)) speed *= SPRINT_MULTIPLIER;
	if (PlatformGetKey(KEY_W)) camera->position += speed * deltaTime * camera->front;
	if (PlatformGetKey(KEY_S)) camera->position -= speed * deltaTime * camera->front;
	if (PlatformGetKey(KEY_A)) camera->position -= camera->front.Cross(Vector3(0.0f, 1.0f, 0.0f)).Normalize() * speed * deltaTime;
	if (PlatformGetKey(KEY_D)) camera->position += camera->front.Cross(Vector3(0.0f, 1.0f, 0.0f)).Normalize()  * speed * deltaTime;
	if (PlatformGetKey(KEY_SPACE)) camera->position.y += speed * deltaTime;
	if (PlatformGetKey(KEY_LCTRL)) camera->position.y -= speed * deltaTime;
	camera->pitch = Clamp(camera->pitch, -89.0f, 89.0f);
}