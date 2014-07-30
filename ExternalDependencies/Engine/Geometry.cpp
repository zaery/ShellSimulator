#include "Geometry.h"
#include "Vertex.h"
#include <ShapeGenerator.h>
#include <ShapeData.h>
void Geometry::setGeometry(const void* verts, const uint numVerts, const void* indices, const uint numIndices)
{
	void* vertPointer = malloc(numVerts*sizeof(Neumont::Vertex));
	memcpy(vertPointer, verts, numVerts*sizeof(Neumont::Vertex));
	this->verts = const_cast<void*>(vertPointer);
	this->verts = const_cast<void*>(verts);
	this->numVerts = numVerts;
	void* indicesPointer = malloc(numIndices*sizeof(ushort));
	memcpy(indicesPointer, indices, numIndices*sizeof(ushort));
	this->indices = const_cast<void*>(indicesPointer);
	this->indices = const_cast<void*>(indices);
	this->numIndices = numIndices;
}

void Geometry::putInBuffers(GLuint vertBuf, GLuint indBuf, GLuint* vertOff, GLuint* indOff)
{
	vBuffer = vertBuf;
	iBuffer = indBuf;
	vOffset = *vertOff;
	iOffset = *indOff;
	
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);

	glBufferSubData(GL_ARRAY_BUFFER, vOffset, numVerts * sizeof(Neumont::Vertex), verts);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, iOffset, numIndices * sizeof(ushort), indices);

	*vertOff = vOffset + numVerts * sizeof(Neumont::Vertex);
	*indOff = iOffset + numIndices * sizeof(ushort);
}