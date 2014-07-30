#ifndef CAMERA_H
#define CAMERA_H

#include <glm\glm.hpp>
#include "ExportHeader.h"

class ENGINE_SHARED Camera
{
private:
	glm::vec3 up;
public:
	float speed;
	float turnSpeed;
	float FOV;
	float nearPlane;
	float farPlane;
	glm::vec3 lookDir;
	glm::vec3 position;
	glm::mat4 getMatrix() const;
	void changeSpeed(float amt);
	void turn(float deg);
	void elevate(float deg);
	void moveForward(float amt);
	void moveBackward(float amt);
	void strafeLeft(float amt);
	void strafeRight(float amt);
	void moveUp(float amt);
	void moveDown(float amt);
	void initialize(const glm::vec3& pos, const glm::vec3& lookAt, 
		const glm::vec3& upDir, float inSpeed, float turnSpeed,
		float fov, float near, float far);
};
#endif