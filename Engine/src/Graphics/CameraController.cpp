#include "CameraController.h"

CameraController::CameraController(Camera* camera, IApplicationInput* input) {
	this->camera = camera;
	this->input = input;
}


CameraController::~CameraController() {

}

void CameraController::Update(float deltaTime) {
	double xpos = input->GetCursorX();
	double ypos = input->GetCursorY();

	static double lastX = xpos;
	static double lastY = ypos;

	float deltaX = xpos - lastX;
	float deltaY = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	Rotate(deltaX, deltaY);

	bool modDown = input->IsKeyDown(INPUT_KEY_LEFT_SHIFT);

	if (input->IsKeyDown(INPUT_KEY_W))
		Move(FOWARD, deltaTime, modDown);
	if (input->IsKeyDown(INPUT_KEY_S))
		Move(BACKWARD, deltaTime, modDown);
	if (input->IsKeyDown(INPUT_KEY_D))
		Move(RIGHT, deltaTime, modDown);
	if (input->IsKeyDown(INPUT_KEY_A))
		Move(LEFT, deltaTime, modDown);
	if (input->IsKeyDown(INPUT_KEY_SPACE))
		Move(DOWN, deltaTime, modDown);
	if (input->IsKeyDown(INPUT_KEY_C))
		Move(UP, deltaTime, modDown);

	camera->Update();
}

void CameraController::Move(CameraMovement direction, float deltaTime, bool modDown = false) {
	float velocity = movementSpeed * deltaTime;
	if (modDown)
		velocity *= velocityScalar;

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

