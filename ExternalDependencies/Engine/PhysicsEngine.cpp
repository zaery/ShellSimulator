#include "PhysicsEngine.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <CL\cl_gl.h>
#include <CL\opencl.h>
#pragma OPENCL EXTENSION cl_khr_gl_sharing enable
#define SUCCESS 0
#define FAILURE 1
PhysicsEngine::PhysicsEngine(std::vector<PointCloud>* clouds){


	//objects = initObjects;
	objects = clouds;
	/*Step1: Getting platforms and choose an available one.*/
	cl_uint numPlatforms;	//the NO. of platforms
	cl_platform_id platform = NULL;	//the chosen platform
	status = clGetPlatformIDs(0, NULL, &numPlatforms);

	/*For clarity, choose the first available platform. */
	if(numPlatforms > 0)
	{
		cl_platform_id* platforms = (cl_platform_id* )malloc(numPlatforms* sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}

	/*Step 2:Query the platform and choose the first GPU device if has one.Otherwise use the CPU as device.*/
	cl_uint				numDevices = 0;
	/*cl_device_id        *devices;*/
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);	
	devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);

	/*Step 3: Create context.*/
	void* glCtx=0;
	void* glDC=0;
	glCtx = wglGetCurrentContext();
	glDC = wglGetCurrentDC();
	cl_context_properties cps[7] = {0,0,0,0,0,0,0};
	cps[0] = CL_CONTEXT_PLATFORM;
	cps[1] = (cl_context_properties)platform;
	cps[2] = CL_GL_CONTEXT_KHR;
	cps[3] = (cl_context_properties)glCtx;
	cps[4] = CL_WGL_HDC_KHR;
	cps[5] = (cl_context_properties)glDC;

	cl_device_id interopDevice;
	clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
	status = clGetGLContextInfoKHR( cps,
		CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
		sizeof(cl_device_id),
		&interopDevice,
		NULL);
	size_t device_size = 0;
	status = clGetGLContextInfoKHR(
		cps, 
		CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, 
		0, 
		NULL, 
		&device_size);




	context = clCreateContext(cps, 1,&interopDevice,0,0, &status);
	//context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);


	/*Step 4: Creating command queue associate with the context.*/
	commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);

	/*Step 5: Create program object */
	const char *filename = "collisionTEST.cl";
	string sourceStr;
	status = convertToString(filename, sourceStr);
	const char *source = sourceStr.c_str();
	size_t sourceSize[] = {strlen(source)};
	program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);

	/*Step 6: Build program. */
	status=clBuildProgram(program, 1,devices,NULL,NULL,NULL);


	size_t log_size;
	clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	// Allocate memory for the log
	char *log = (char *) malloc(log_size);
	// Get the log
	clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

	cloudPositions[0] = clCreateBuffer(context, CL_MEM_READ_ONLY&&CL_MEM_COPY_HOST_PTR, sizeof(glm::vec4), &objects->at(0).position, &status);
	//positions[0] = clCreateBuffer(context, CL_MEM_READ_ONLY&&CL_MEM_COPY_HOST_PTR, sizeof(glm::vec4)*objects->at(0).points.size(), &objects->at(0).points, &status);
	positions[0] = clCreateFromGLBuffer(context, CL_MEM_READ_ONLY&&CL_MEM_COPY_HOST_PTR, objects->at(0).vBuffer, &status);
	numPoints[0] = clCreateBuffer(context, CL_MEM_READ_ONLY&&CL_MEM_COPY_HOST_PTR, sizeof(float), &objects->at(0).numPoints, &status);
	
	cloudPositions[1] = clCreateBuffer(context, CL_MEM_READ_ONLY&&CL_MEM_COPY_HOST_PTR, sizeof(glm::vec4), &objects->at(1).position, &status);
	//positions[1] = clCreateBuffer(context, CL_MEM_READ_ONLY&&CL_MEM_COPY_HOST_PTR, sizeof(glm::vec4)*objects->at(1).points.size(), &objects->at(1).points, &status);
	positions[1] = clCreateFromGLBuffer(context, CL_MEM_READ_ONLY&&CL_MEM_COPY_HOST_PTR, objects->at(1).vBuffer, &status);
	numPoints[1] = clCreateBuffer(context, CL_MEM_READ_ONLY&&CL_MEM_COPY_HOST_PTR, sizeof(float), &objects->at(1).numPoints, &status);

	size_t bufferSize = sizeof(cl_float4)*objects->at(0).points.size();
	newPositions[0] = clCreateBuffer(context, CL_MEM_READ_WRITE&&CL_MEM_COPY_HOST_PTR, bufferSize, 0, &status);

	float* p = (float*) clEnqueueMapBuffer(commandQueue, newPositions[0], CL_TRUE, CL_MAP_WRITE,
		0, bufferSize, 0, NULL, NULL, &status);
	memset(p, 0, bufferSize);
	status = clEnqueueUnmapMemObject(commandQueue, newPositions[0], p, 0, NULL, NULL);
	/*Step 8: Create kernel object */
	detectCollision = clCreateKernel(program,"collisionTest", NULL);
	resolveStructure = clCreateKernel(program,"resolveStructure", NULL);
	integrate = clCreateKernel(program,"integrate", NULL);
	clFlush(commandQueue);
	clFinish(commandQueue);
}

PhysicsEngine::~PhysicsEngine(){
	/*Step 12: Clean the resources.*/
	status = clReleaseKernel(detectCollision);				//Release kernel.
	status = clReleaseProgram(program);				//Release the program object.

	status = clReleaseCommandQueue(commandQueue);	//Release  Command queue.
	status = clReleaseContext(context);				//Release context.
}

void PhysicsEngine::update(float deltaT){
	glFinish();
	status = clEnqueueAcquireGLObjects(commandQueue, 1, &positions[0], 0, 0, 0);
	status = clEnqueueAcquireGLObjects(commandQueue, 1, &positions[1], 0, 0, 0);

	size_t kernel_work_group_size[2] = {512, 512};

	//detect collisions
	status = clSetKernelArg(detectCollision, 0, sizeof(cl_float4), &objects->at(0).position);
	status = clSetKernelArg(detectCollision, 1, sizeof(cl_mem), (void *)&positions[0]);
	status = clSetKernelArg(detectCollision, 2, sizeof(unsigned int), (void *)&objects->at(0).numPoints);
	status = clSetKernelArg(detectCollision, 3, sizeof(cl_float4), &objects->at(1).position);
	status = clSetKernelArg(detectCollision, 4, sizeof(cl_mem), (void *)&positions[1]);
	status = clSetKernelArg(detectCollision, 5, sizeof(unsigned int), &objects->at(1).numPoints);
	status = clSetKernelArg(detectCollision, 6, sizeof(cl_mem), (void *)newPositions);
	status = clEnqueueNDRangeKernel(commandQueue, detectCollision, 2, NULL, 
		&kernel_work_group_size[0], NULL, 0, NULL, NULL);
	
	//Structure 0
	status = clSetKernelArg(resolveStructure, 0, sizeof(cl_mem), (void *)&positions[0]);
	status = clSetKernelArg(resolveStructure, 1, sizeof(unsigned int), (void *)&objects->at(0).numPoints);
	status = clEnqueueNDRangeKernel(commandQueue, resolveStructure, 2, NULL,
		&kernel_work_group_size[0], NULL, 0, NULL, NULL);

	//Structure 1
	status = clSetKernelArg(resolveStructure, 0, sizeof(cl_mem), (void *)&positions[1]);
	status = clSetKernelArg(resolveStructure, 1, sizeof(unsigned int), (void *)&objects->at(1).numPoints);
	status = clEnqueueNDRangeKernel(commandQueue, resolveStructure, 2, NULL,
		&kernel_work_group_size[0], NULL, 0, NULL, NULL);

	//Integrate 0
	status = clSetKernelArg(integrate, 0, sizeof(cl_mem), (void *)&positions[0]);
	status = clSetKernelArg(integrate, 1, sizeof(unsigned int), (void *)&objects->at(0).numPoints);
	status = clEnqueueNDRangeKernel(commandQueue, integrate, 2, NULL,
		&kernel_work_group_size[0], NULL, 0, NULL, NULL);

	//Integrate 1
	status = clSetKernelArg(integrate, 0, sizeof(cl_mem), (void *)&positions[1]);
	status = clSetKernelArg(integrate, 1, sizeof(unsigned int), (void *)&objects->at(1).numPoints);
	status = clEnqueueNDRangeKernel(commandQueue, integrate, 2, NULL,
		&kernel_work_group_size[0], NULL, 0, NULL, NULL);

	status = clEnqueueReleaseGLObjects(commandQueue, 1, &positions[0], 0,0,0);
	status = clEnqueueReleaseGLObjects(commandQueue, 1, &positions[1], 0,0,0);
	status = clFinish(commandQueue);
}

/* convert the kernel file into a string */
int PhysicsEngine::convertToString(const char *filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if(f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);
		str = new char[size+1];
		if(!str)
		{
			f.close();
			return 0;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return 0;
	}
	cout<<"Error: failed to open file\n:"<<filename<<endl;
	return FAILURE;
}