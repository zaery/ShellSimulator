#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <GL\glew.h>
#include "ExportHeader.h"
#include "TypeDefs.h"
class ENGINE_SHARED Geometry
{
public:
	GLuint vBuffer;
	GLuint iBuffer;
	GLuint vOffset;
	GLuint iOffset;
	void* verts;
	uint numVerts;
	void* indices;
	uint numIndices;
	void setGeometry(const void* inverts, const uint innumVerts, const void* inindices, const uint innumIndices);
	void putInBuffers(GLuint vertBuf, GLuint indBuf, GLuint* vertOff, GLuint* indOff);
};
#endif