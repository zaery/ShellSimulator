#ifndef RENDERABLE_H
#define RENDERABLE_H
#include <glm\glm.hpp>
#include <GL\glew.h>
#include "TypeDefs.h"
#include "ExportHeader.h"
#include <memory>

class Geometry;

enum UniformType {GLM1IV, GLM1FV, GLM2FV, GLM3FV, GLM4FV, GLMMAT2, GLMMAT3, GLMMAT4};

class ENGINE_SHARED Renderable
{
	uint drawMode;
	uint how;
	bool depth;
	static const ushort MAX_NUM_UNIFORMS = 10;
	char* uniformNames[MAX_NUM_UNIFORMS];
	void* uniformDatas[MAX_NUM_UNIFORMS];
	int uniformTypes[MAX_NUM_UNIFORMS];
	int numUniforms;
	void useUniforms();
	void applyUniform(const char* name, void* data, const int& uniformType);
public:
	bool hasLife;
	float lifeRemaining;
	void setLife(const float& life);
	glm::mat4 where;
	Geometry* geo;
	void setDrawMode(const uint& mode);
	void addUniform(const char* name, const void* data, int uniformType);
	void init(const Geometry* g, const glm::mat4& transform, uint id, bool dep = true, uint mode = GL_TRIANGLES);
	void setGeometry(const Geometry* g);
	void setWhere(const glm::mat4& where);
	void setShader(uint id);
	void paint(const glm::mat4& camMat, const glm::mat4& projection);
};

#endif