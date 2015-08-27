#include "Camera.hpp"

Camera::Camera(float viewportWidth, float viewportHeight) {
	position.Set(0.0f, 0.0f, 3.0f);
	direction.Set(0.0f, 0.0f, -1.0f);
	up.Set(0.0f, 1.0f, 0.0f);

	worldUp = Vector3(0.0f, 1.0f, 0.0f);

	yaw = -90.0f;
	pitch = 0.0f;

	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
}

Camera::~Camera() {

}

void Camera::Translate(Vector3& deltaPos) {
	position += deltaPos;
}

void Camera::SetPosition(Vector3* position) {
	this->position.Set(position);
}

void Camera::LookAt(Vector3* position, Vector3* direction, Vector3* up) {
	this->position.Set(position);
	this->direction.Set(direction);
	this->up.Set(up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
	return projection;
}
glm::mat4 Camera::GetViewMatrix() const {
	return view;
}
glm::mat4 Camera::GetCombinedMatrix() const{
	return projection * view;
}

void Camera::Update() {
	UpdateCameraVectors();
}

void Camera::UpdateCameraVectors() {
	front.x = cosf(MathUtils::Radians(yaw)) * cosf(MathUtils::Radians(pitch));
	front.y = sinf(MathUtils::Radians(pitch));
	front.z = sinf(MathUtils::Radians(yaw)) * cosf(MathUtils::Radians(pitch));
	front.Normalize();
	
	right = front.Cross(worldUp);
	right.Normalize();

	up = right.Cross(front);
	up.Normalize();
}

PerspectiveCamera::PerspectiveCamera(float viewportWidth, float viewportHeight) : Camera(viewportWidth, viewportHeight) {
	fov = 45.0f;
	zNear = 0.1f;
	zFar = 1000;
}

void PerspectiveCamera::Update() {
	Camera::UpdateCameraVectors();
	projection = glm::perspective(fov, (viewportWidth / viewportHeight), zNear, zFar);
	view = lookAt(position.ToGLM(), position.ToGLM() + front.ToGLM(),up.ToGLM());
}