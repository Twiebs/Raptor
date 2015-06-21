// #include "FPSCamera.h"
//
// bool FPSCamera::OnKeyEvent(int keycode, bool isPressed, int mods) {
// 	switch (keycode) {
//
// 	case Key::W:
// 		isMovingFoward = isPressed;
// 		return true;
//
// 	case Key::S:
// 		isMovingBackward = isPressed;
// 		return true;
//
// 	case Key::D:
// 		isMovingRight = isPressed;
// 		return true;
//
// 	case Key::A:
// 		isMovingLeft = isPressed;
// 		return true;
//
// 	case Key::SPACE:
// 		isMovingUp = isPressed;
// 		return true;
// 	case Key::C:
// 		isMovingDown = isPressed;
// 		return true;
//
// 	case Key::LEFT_SHIFT:
// 		isSprintActive = isPressed;
// 		return false;	//Other listeners will pick this up as well.
//
// 	case Key::LEFT_ALT:
// 		isHyperDriveActive = isPressed;
// 		return false;
// 	}
// 	return false;
// }
// bool FPSCamera::OnCursorPosEvent(double xpos, double ypos) {
// 	static double lastX = xpos;
// 	static double lastY = ypos;
//
// 	float deltaX = xpos - lastX;
// 	float deltaY = lastY - ypos;
//
// 	yaw += deltaX * lookSensitivity;
// 	pitch += deltaY * lookSensitivity;
// 	pitch = MathUtils::Clamp(pitch, -89.0f, 89.0f);
//
// 	lastX = xpos;
// 	lastY = ypos;
// 	return true;
// }
//
// void FPSCamera::Update(float deltaTime) {
// 	PerspectiveCamera::Update();
// 	float velocity = movementSpeed * deltaTime;
//
// 	if (isSprintActive)
// 		velocity *= velocityScalar;
// 	if (isHyperDriveActive)
// 		velocity *= (velocityScalar * velocityScalar);
//
//
// 	if (isMovingFoward)
// 		position += front * velocity;
// 	if (isMovingBackward)
// 		position -= front * velocity;
// 	if (isMovingRight)
// 		position += right * velocity;
// 	if (isMovingLeft)
// 		position -= right * velocity;
// 	if (isMovingDown)
// 		position -= worldUp * velocity;
// 	if (isMovingUp)
// 		position += worldUp * velocity;
//
// 	PerspectiveCamera::Update();
// }
