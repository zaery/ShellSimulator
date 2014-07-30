#include "Renderable.h"
#include <Qt\qdebug.h>
#include "Geometry.h"
#include "Vertex.h"

void Renderable::setGeometry(const Geometry* g){
	geo = const_cast<Geometry*>(g);
}

void Renderable::setWhere(const glm::mat4& where){
	this->where = where;
}

void Renderable::setShader(uint id){
	how = id;
}

void Renderable::useUniforms(){
	for (int i=0;i<numUniforms;i++){
		applyUniform(uniformNames[i], uniformDatas[i], uniformTypes[i]);
	}
}

void Renderable::applyUniform(const char* name, void* data, const int& uniformType){
	GLint uniformLoc = glGetUniformLocation(how, name);
	if (uniformLoc >= 0){
		switch(uniformType){
		case GLM1IV:
			glUniform1iv(uniformLoc, 1, reinterpret_cast<GLint*>(const_cast<void*>(data)));
			break;
		case GLM1FV:
			glUniform1fv(uniformLoc, 1, reinterpret_cast<GLfloat*>(const_cast<void*>(data)));
			break;
		case GLM2FV:
			glUniform2fv(uniformLoc, 2, reinterpret_cast<GLfloat*>(const_cast<void*>(data)));
			break;
		case GLM3FV:
			glUniform3fv(uniformLoc, 3, reinterpret_cast<GLfloat*>(const_cast<void*>(data)));
			break;
		case GLM4FV:
			glUniform4fv(uniformLoc, 4, reinterpret_cast<GLfloat*>(const_cast<void*>(data)));
			break;
		case GLMMAT2:
			glUniformMatrix2fv(uniformLoc, 1, GL_FALSE, reinterpret_cast<GLfloat*>(const_cast<void*>(data)));
			break;
		case GLMMAT3:
			glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, reinterpret_cast<GLfloat*>(const_cast<void*>(data)));
			break;
		case GLMMAT4:
			glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, reinterpret_cast<GLfloat*>(const_cast<void*>(data)));
			break;
		}
	}
}

void Renderable::addUniform(const char* name, const void* data, int uniformType){
	if (numUniforms < MAX_NUM_UNIFORMS){
		uniformNames[numUniforms] = const_cast<char*>(name);
		uniformDatas[numUniforms] = const_cast<void*>(data);
		uniformTypes[numUniforms] = uniformType;
		++numUniforms;
	}
}

void Renderable::setDrawMode(const uint& mode){
	drawMode = mode;
}

void Renderable::init(
	const Geometry* g, 
	const glm::mat4& transform, 
	uint id, 
	bool dep, 
	uint mode){
	geo = const_cast<Geometry*>(g);
	where = transform;
	how = id;
	drawMode = mode;
	hasLife = false;
	depth = dep;
	numUniforms = 0;
}

void Renderable::setLife(const float& life){
	hasLife = true;
	lifeRemaining = life;
}

void Renderable::paint(const glm::mat4& camMat, const glm::mat4& projection){
	glBindBuffer(GL_ARRAY_BUFFER, geo->vBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geo->iBuffer);
	glUseProgram(how);
	glm::mat4 fullTransform = projection * camMat * where;

	useUniforms();
	
	GLint mvpUniformLocation = glGetUniformLocation(how, "transform");
	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
		Neumont::Vertex::STRIDE,
		BUFFER_OFFSET(Neumont::Vertex::POSITION_OFFSET+geo->vOffset));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
		Neumont::Vertex::STRIDE,
		BUFFER_OFFSET(Neumont::Vertex::COLOR_OFFSET+geo->vOffset));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		Neumont::Vertex::STRIDE,
		BUFFER_OFFSET(Neumont::Vertex::NORMAL_OFFSET+geo->vOffset));

	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	if (!depth)
		glDepthFunc( GL_ALWAYS );
	glDrawElements(
		drawMode, geo->numIndices, GL_UNSIGNED_SHORT, 
		BUFFER_OFFSET(geo->iOffset));
	if (!depth)
		glDepthFunc( GL_LESS );
}