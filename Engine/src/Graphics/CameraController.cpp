#include "CameraController.h"

CameraController::CameraController() {

}
CameraController::~CameraController() {

}

void CameraController::SetCamera(Camera* camera) {
	this->camera = camera;
}

bool CameraController::OnKeyDown(int button, int mod) {
	switch (button) {
	case INPUT_KEY_W: 
		foward = true; 
		return true;
	case INPUT_KEY_S: 
		backward = true; 
		return true;
	case INPUT_KEY_D: 
		right = true;
		return true;
	case INPUT_KEY_A: 
		left = true;
		return true;
	case INPUT_KEY_SPACE: 
		up = true;
		return true;
	case INPUT_KEY_C: 
		down = true;
		return true;
	case INPUT_KEY_LEFT_SHIFT: 
		sprintIsActive = true;
		return true;
	}
}

bool CameraController::OnCursorPos(double xpos, double ypos) {
	static double lastX = xpos;
	static double lastY = ypos;
	
	deltaX = xpos - lastX;
	deltaY = ypos - lastY;
	lastX = xpos;
	lastY = ypos;
	return true;
}



void CameraController::Update(float deltaTime) {
	Rotate(deltaX, deltaY);

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
		camera->position -= camera->worldUp * velocity;
		break;
	case DOWN:
		camera->position += camera->worldUp * velocity;
	}
}

void CameraController::Rotate(float deltaX, float deltaY) {
	deltaX *= lookSensitivity;
	deltaY *= lookSensitivity;

	camera->yaw += deltaX;
	camera->pitch += deltaY;
	
	camera->pitch = MathUtils::Clamp(camera->pitch, -89.0f, 89.0f);
}

