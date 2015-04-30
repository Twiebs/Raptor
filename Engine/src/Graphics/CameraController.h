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

	void SetCamera(Camera* camera);
	bool OnKeyDown(int button, int mod) override;
	bool OnCursorPos(double xPos, double yPos) override;

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

	float deltaX, deltaY;

	Camera* camera;

	float movementSpeed = 4.0f;
	float lookSensitivity = 0.75f;
	float velocityScalar = 10.5f;

};

