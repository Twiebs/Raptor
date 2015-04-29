#pragma once

#include<Core\Input.h>
#include <Graphics\Camera.h>
#include <Math\MathUtils.h>

enum CameraMovement {
	FOWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class CameraController {
public:
	CameraController(Camera* camera, InputProcessor* inputProcessor);
	~CameraController();

	void Move(CameraMovement direction, float deltaTime, bool modDown);
	void Rotate(float deltaX, float deltaY);

	void Update(float deltaTime);

private:
	Camera* camera;
	InputProcessor* inputProcessor;
	float movementSpeed = 4.0f;
	float lookSensitivity = 0.75f;
	float velocityScalar = 10.5f;

};

