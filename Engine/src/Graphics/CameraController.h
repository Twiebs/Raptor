#pragma once

#include<Core\Input.h>
#include<Graphics\Camera.h>
#include <Math\MathUtils.h>

class Engine;

enum CameraMovement {
	FOWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class CameraController : public IInputListener{
public:
	CameraController();
	~CameraController();

	//InputProcessing
	bool OnKeyEvent(int keycode, bool isPressed, int mod) override;
	bool OnCursorPosEvent(double xPos, double yPos) override;

	void SetCamera(Camera* camera);
	void Move(CameraMovement direction, float deltaTime);
	void Rotate(float deltaX, float deltaY);
	void Update(float deltaTime);

private:
	bool sprintIsActive;
	bool foward;
	bool backward;
	bool left;
	bool right;
	bool up;
	bool down;

	Camera* camera;
	float movementSpeed = 4.0f;
	float lookSensitivity = 0.75f;
	float velocityScalar = 10.5f;

};

