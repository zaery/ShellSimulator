#ifndef RENDERER_H
#define RENDERER_H
#include "Camera.h"
#include "Geometry.h"
#include "Renderable.h"
#include "Timer.h"
#include "ExportHeader.h"
#include "LuaState.h"
#include "PointCloud.h"
using utils::LuaState;
class ENGINE_SHARED Renderer
{
	bool splitscreen;
	Timer timer;
	static const ushort MAX_NUM_VERTEX_BUFFERS = 10;
	static const ushort MAX_NUM_INDEX_BUFFERS = 10;
	ushort currentBuffer;
	uint vBuffer[MAX_NUM_VERTEX_BUFFERS];
	uint vOffsets[MAX_NUM_VERTEX_BUFFERS];
	uint iBuffer[MAX_NUM_INDEX_BUFFERS];
	uint iOffsets[MAX_NUM_INDEX_BUFFERS];
public:
	static const uint MAX_NUM_RENDERABLES = 200;
	static const uint MAX_NUM_GEOMETRIES = 50;
	uint lastThing;
	Geometry geometries[MAX_NUM_GEOMETRIES];
	uint lastGeo;
	std::vector<PointCloud>* points;
private:
	uint width;
	uint height;
	void initBuffers();
	uint createShaders(const char* vertShaderName, const char* fragShaderName);
	void debugBufferShapes();
	void readSomeAwesomeStuffFromAFile();
	void compileShader(const char* fileName, GLuint shaderID);
public:
	uint currentProgramID;
	uint programID[10];
	const Geometry* makeSphereGeom();
	const Geometry* makeLineGeom();
	void removeRenderable(Renderable* removal);
	Renderable things[MAX_NUM_RENDERABLES];
	void update();
	Camera camera;
	bool hasSwapped;
	Camera secondaryCamera;
	void setWindowDims(uint wid, uint hei);
	void paint();
	glm::mat4 align(const glm::vec3& originalOrientation, const glm::vec3& newOrientation);
	void init(std::vector<PointCloud>* pointClouds);
	Geometry* addGeometry(const void* verts, const uint numVerts, const void* indices, const uint numIndices);
	Renderable* addRenderable(
		const Geometry* geo,
		const glm::mat4& where,
		uint how,
		bool depth = true,
		uint drawMode = GL_TRIANGLES,
		float life = 0.0f);
	uint addShader(const char* vertShaderFileName, const char* fragShaderFileName);
};

#endif