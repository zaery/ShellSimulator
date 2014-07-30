#include <glm\gtx\transform.hpp>
#include "Camera.h"

void Camera::initialize(const glm::vec3& pos, const glm::vec3& lookAt, const glm::vec3& upDir, float inSpeed, float inTurnSpeed,
		float fov, float near, float far){
	position = pos;
	lookDir = lookAt;
	up = upDir;
	speed = inSpeed;
	turnSpeed = inTurnSpeed;
	FOV = fov;
	nearPlane = near;
	farPlane = far;
}

glm::mat4 Camera::getMatrix() const{
	return glm::lookAt(
		position,
		position + lookDir,
		up);
}

void Camera::changeSpeed(float amt){
	speed *= amt;
}

void Camera::turn(float deg){
	glm::vec4 temp = glm::vec4(lookDir, 0.0f);
	temp = temp*glm::rotate(deg * turnSpeed, glm::vec3(0,1,0));
	lookDir = glm::vec3(temp.x, temp.y, temp.z);
}

void Camera::elevate(float deg){
	glm::vec4 temp = glm::vec4(lookDir, 0.0f) * glm::rotate(deg * turnSpeed, glm::cross(lookDir, up));
	lookDir = glm::vec3(temp.x, temp.y, temp.z);
}

void Camera::moveForward(float amt){
	position = position + speed * amt * glm::normalize(glm::vec3(lookDir.x, 0, lookDir.z));
}

void Camera::moveBackward(float amt){
		position = position - speed * amt * glm::normalize(glm::vec3(lookDir.x, 0, lookDir.z));
}

void Camera::strafeLeft(float amt){
	glm::vec4 temp = glm::vec4(lookDir.x, 0.0f, lookDir.z, 0.0f);
	temp = speed * amt * glm::normalize(temp * glm::rotate(-90.0f, glm::vec3(0,1,0)));
	position = glm::vec3(position.x + temp.x, position.y, position.z + temp.z);
}

void Camera::strafeRight(float amt){
	glm::vec4 temp = glm::vec4(lookDir.x, 0.0f, lookDir.z, 0.0f);
	temp = speed * amt * glm::normalize(temp * glm::rotate(90.0f, glm::vec3(0,1,0)));
	position = glm::vec3(position.x + temp.x, position.y, position.z + temp.z);
}

void Camera::moveUp(float amt){
	position = position + speed * glm::vec3(0.0f, amt, 0.0f);
}

void Camera::moveDown(float amt){
	position = position - speed * glm::vec3(0.0f, amt, 0.0f);
}