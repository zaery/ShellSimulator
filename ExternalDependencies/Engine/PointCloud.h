#pragma once
#include <vector>
#include <glm\glm.hpp>
#include <GL\glew.h>
#include "TypeDefs.h"
#include "Point.h"
class PointCloud{
public:
	uint how;
	GLuint vBuffer;
	GLuint iBuffer;
	GLuint vOffset;
	GLuint iOffset;
	std::vector<Point> points;
	uint numPoints;
	//Octree<glm::vec3> pointOctree;
	PointCloud();
	PointCloud(float width);
	glm::vec4 position;
	void paint(const glm::mat4& camMat, const glm::mat4& projection);
	void putInBuffers(uint shaderID, GLuint vertBuf, GLuint indBuf, GLuint* vertOff, GLuint* indOff);
};