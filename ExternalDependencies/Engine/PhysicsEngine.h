#pragma once
#include "GameObject.h"
#include "PhysicsComponent.h"
#include "PointCloud.h"
#include <CL\cl.hpp>

#define clGetGLContextInfoKHR clGetGLContextInfoKHR_proc
static clGetGLContextInfoKHR_fn clGetGLContextInfoKHR;

class PhysicsEngine
{
	std::vector<PointCloud>* objects;
	cl_int status;
	cl_kernel detectCollision;
	cl_kernel resolveStructure;
	cl_kernel integrate;
	cl_program program;
	cl_mem cloudPositions[2];
	cl_mem newPositions[1];
	cl_mem positions[2];
	cl_mem numPoints[2];
	cl_command_queue commandQueue;
	cl_context context;
	//char* output;
	cl_device_id* devices;
public:
	PhysicsEngine(std::vector<PointCloud>* clouds);
	void addPointCloud(shared_ptr<PointCloud> cloud);
	~PhysicsEngine();
	void update(float deltaT);
	int convertToString(const char *filename, std::string& s);
};