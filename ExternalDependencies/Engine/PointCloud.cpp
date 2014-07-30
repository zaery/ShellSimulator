#include "PointCloud.h"
#include <stdio.h>
#include <glm\gtx\transform.hpp>
PointCloud::PointCloud(){
	//pointOctree = Octree<glm::vec3>(64, glm::vec3());
	numPoints=0;
	/*for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			for (int k = 0; k < 4; k++){
				points.push_back(glm::vec4(i,j,k, 1.0f));
				numPoints++;
			}
		}
	}*/

	FILE* pFile;
	pFile = fopen("Bullet.xyz", "r");
	bool status = true;
	int ret = 0;
	float x;
	float y;
	float z;
	while (status){
		ret = fscanf(pFile, "%f", &x);
		ret = fscanf(pFile, "%f", &y);
		ret = fscanf(pFile, "%f", &z);
		if (ret==-1){
			status = false;
		}else{

			points.push_back(Point(glm::vec4(x,y,z,50.0f), glm::vec4(0.0f, 0.0f, 0.04f, 0.0f), glm::vec4(), glm::vec4()));
			numPoints++;
		}
	}
}

PointCloud::PointCloud(float width){
	numPoints=0;
	for (int i = 0; i < 40; i++){
		for (int j = 0; j < 40; j++){
			for (int k = 0; k < width; k++){
				points.push_back(Point(glm::vec4(i*0.2f,j*0.2f,k*0.2f, 10.0f), glm::vec4(), glm::vec4(), glm::vec4()));
				numPoints++;
			}
		}
	}
}

void PointCloud::paint(const glm::mat4& camMat, const glm::mat4& projection){
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
	glUseProgram(how);
	glm::mat4 fullTransform = projection * camMat * glm::translate(glm::vec3(position));
	glPointSize(4.0f);
	GLint mvpUniformLocation = glGetUniformLocation(how, "transform");
	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 
		64,
		BUFFER_OFFSET(vOffset));

	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &fullTransform[0][0]);
	glDrawElements(
		GL_POINTS, points.size(), GL_UNSIGNED_INT, 
		BUFFER_OFFSET(iOffset));
}

void PointCloud::putInBuffers(uint shaderID, GLuint vertBuf, GLuint indBuf, GLuint* vertOff, GLuint* indOff)
{
	how = shaderID;
	vBuffer = vertBuf;
	iBuffer = indBuf;
	vOffset = *vertOff;
	iOffset = *indOff;
	
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);

	glBufferSubData(GL_ARRAY_BUFFER, vOffset, points.size() * sizeof(Point), (&points[0]));
	std::vector<uint> indices;
	for (uint i=0;i<points.size();i++){
		indices.push_back(i);
	}
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, iOffset, points.size() * sizeof(uint), indices.data());

	*vertOff = vOffset + points.size() * sizeof(Point);
	*indOff = iOffset + points.size() * sizeof(uint);
}