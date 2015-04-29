#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include<Math\MathUtils.h>
#include<Math\Vector3.h>

class Camera {
public:
	float viewportWidth, viewportHeight;
	Vector3 position;

	Vector3 front;
	Vector3 up;
	Vector3 right;

	float yaw;
	float pitch;

	Vector3 direction;
	Vector3 worldUp;

	Camera(float viewportWidth, float viewportHeight);
	~Camera();

	void Translate(Vector3& deltaPos);
	void SetPosition(Vector3* position);
	void LookAt(Vector3* position, Vector3* direction, Vector3* up);

	const Vector3& GetPosition() const;
	const Vector3& GetTarget() const;
	const Vector3& GetUp() const;

	glm::mat4 GetProjectionMatrix() const;
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetCombinedMatrix() const;

	virtual void Update();
protected:
	glm::mat4 projection;
	glm::mat4 view;

	void UpdateCameraVectors();
};

class PerspectiveCamera : public Camera {
public:
	float fov;
	float zNear, zFar;
	PerspectiveCamera(float viewportWidth, float viewportHeight);

	virtual void Update();

private:

};

