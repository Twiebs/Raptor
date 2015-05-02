#include "CameraController.h"

CameraController::CameraController() {

}
CameraController::~CameraController() {

}

void CameraController::SetCamera(Camera* camera) {
	this->camera = camera;
}

bool CameraController::OnKeyEvent(int keycode, bool isPressed, int mod) {
	switch (keycode) {
	case INPUT_KEY_W: foward = isPressed;	return true;
	case INPUT_KEY_S: backward = isPressed; return true;
	case INPUT_KEY_D: right = isPressed;	return true;
	case INPUT_KEY_A: left = isPressed;		return true;
	case INPUT_KEY_SPACE: up = isPressed;	return true;
	case INPUT_KEY_C: down = isPressed;		return true;
	case INPUT_KEY_LEFT_SHIFT: sprintIsActive = isPressed;return true;
	}

	return false;
}

bool CameraController::OnCursorPosEvent(double xpos, double ypos) {
	static double lastX = xpos;
	static double lastY = ypos;
	
	float deltaX = xpos - lastX;
	float deltaY = lastY - ypos;

	camera->yaw += deltaX * lookSensitivity;
	camera->pitch += deltaY * lookSensitivity;
	camera->pitch = MathUtils::Clamp(camera->pitch, -89.0f, 89.0f);

	lastX = xpos;
	lastY = ypos;
	return true;
}



void CameraController::Update(float deltaTime) {
	if (foward) Move(FOWARD, deltaTime);
	if (backward) Move(BACKWARD, deltaTime);
	if (right) Move(RIGHT, deltaTime);
	if (left) Move(LEFT, deltaTime);
	if (down) Move(DOWN, deltaTime);
	if (up) Move(UP, deltaTime);
	camera->Update();
}

void CameraController::Move(CameraMovement direction, float deltaTime) {
	float velocity = movementSpeed * deltaTime;
	if (sprintIsActive) velocity *= velocityScalar;

	switch (direction) {
	case FOWARD:
		camera->position += camera->front * velocity;
		break;
	case BACKWARD:
		camera->position -= camera->front * velocity;
		break;
	case LEFT:
		camera->position -= camera->right * velocity;
		break;
	case RIGHT:
		camera->position += camera->right * velocity;
		break;
	case UP:
		camera->position += camera->worldUp * velocity;
		break;
	case DOWN:
		camera->position -= camera->worldUp * velocity;
		break;
	}
}

void CameraController::Rotate(float deltaX, float deltaY) {
	camera->yaw += deltaX * lookSensitivity;
	camera->pitch += deltaY * lookSensitivity;
	camera->pitch = MathUtils::Clamp(camera->pitch, -89.0f, 89.0f);
}

