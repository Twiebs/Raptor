// #pragma once
//
// #include <Core/IInputListener.hpp>
// #include <Core/InputService.hpp>
// #include <Graphics/Camera.h>
//
// //A pserspective camera with built in FPS style camera controls
// class FPSCamera : public PerspectiveCamera, public IInputListener{
// public:
// 	FPSCamera(float viewportWidth, float viewportHeight) : PerspectiveCamera(viewportWidth, viewportHeight) {
// 		this->zFar = 5000000;
// 	}
// 	~FPSCamera() { }
//
// 	//Moves the camera
// 	bool OnKeyEvent(int key, bool isPressed, int mods) override;
// 	//Rotates the camera
// 	bool OnCursorPosEvent(double xpos, double ypos) override;
// 	//Moves the camera and updates the matrices via the base class
// 	void Update(float deltaTime);
// 
// private:
// 	//Configuration
// 	float movementSpeed = 4.0f;
// 	float lookSensitivity = 0.75f;
// 	float velocityScalar = 10.5f;
//
// 	//State of the controller
// 	//TODO check if using a bitmask to perserve state will be faster?
// 	bool isSprintActive = false;
// 	bool isHyperDriveActive = false;
// 	bool isMovingFoward	= false;
// 	bool isMovingBackward = false;
// 	bool isMovingLeft = false;
// 	bool isMovingRight = false;
// 	bool isMovingUp = false;
// 	bool isMovingDown = false;
// };
