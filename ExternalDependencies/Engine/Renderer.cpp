#include "Renderer.h"
#include <Qt\qdebug.h>
#include <Qt\qfile.h>
#include <glm\gtx\transform.hpp>
#include <fstream>
#include <ShapeData.h>
#include <ShapeGenerator.h>
#include "LuaState.h"
void Renderer::init(std::vector<PointCloud>* pointClouds){
	LuaState* L = new LuaState("config.lua");
	timer.Start();
	timer.Interval();
	camera.initialize(glm::vec3(L->get<float>("camera.posX"), L->get<float>("camera.posY"), L->get<float>("camera.posZ")), 
		glm::vec3(L->get<float>("camera.lookX"),L->get<float>("camera.lookY"),L->get<float>("camera.lookZ")), 
		glm::vec3(L->get<float>("camera.upX"), L->get<float>("camera.upY"), L->get<float>("camera.upZ")), 
		L->get<float>("camera.baseSpeed"), L->get<float>("camera.turnSpeed"),
		L->get<float>("camera.FOV"), L->get<float>("camera.nearPlane"), L->get<float>("camera.farPlane"));
	camera.changeSpeed(L->get<float>("camera.speedMultiplier"));
	currentProgramID = 0;
	createShaders("PassThrough.vsh", "PassThrough.fsh");
	//createShaders("UniformColor.vsh", "UniformColor.fsh");
	//createShaders("normalColors.vsh", "normalColors.fsh");
	//createShaders("PositionColor.vsh", "PositionColor.fsh");
	//createShaders("Color.vsh", "Color.fsh");
	initBuffers();
	lastThing = 0;
	lastGeo = 0;
	for (int i=0;i<10;i++){
		vOffsets[i] = 0;
		iOffsets[i] = 0;
	}
	splitscreen = false;
	hasSwapped = false;

	points = pointClouds;

	++currentBuffer;
	glGenBuffers(1, &vBuffer[currentBuffer]);
	glGenBuffers(1, &iBuffer[currentBuffer]);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer[currentBuffer]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer[currentBuffer]);
	glBufferData(GL_ARRAY_BUFFER, 0x100000, 0, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0x100000, 0, GL_STATIC_DRAW);
	points->at(0).putInBuffers(programID[0], vBuffer[currentBuffer], iBuffer[currentBuffer], &vOffsets[currentBuffer], &iOffsets[currentBuffer]);
	points->at(0).position = glm::vec4(4,4,-2,100);

	
	++currentBuffer;
	glGenBuffers(1, &vBuffer[currentBuffer]);
	glGenBuffers(1, &iBuffer[currentBuffer]);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer[currentBuffer]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer[currentBuffer]);
	glBufferData(GL_ARRAY_BUFFER, 0x100000, 0, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0x100000, 0, GL_STATIC_DRAW);
	points->at(1).putInBuffers(programID[0], vBuffer[currentBuffer], iBuffer[currentBuffer], &vOffsets[currentBuffer], &iOffsets[currentBuffer]);
	points->at(1).position = glm::vec4(0,0,5,1);
}

void Renderer::initBuffers(){
	glGenBuffers(1, &vBuffer[0]);
	glGenBuffers(1, &iBuffer[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer[0]);

	glBufferData(GL_ARRAY_BUFFER, 0x100000, 0, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0x100000, 0, GL_STATIC_DRAW);
	currentBuffer = 0;
}

const Geometry* Renderer::makeSphereGeom(){
	Neumont::ShapeData temp = Neumont::ShapeGenerator::makeSphere(10);
	addGeometry(temp.verts, temp.numVerts, temp.indices, temp.numIndices);
	return &geometries[lastGeo-1];
}

const Geometry* Renderer::makeLineGeom(){
	Neumont::ShapeData temp = Neumont::ShapeGenerator::makeVector(10);
	addGeometry(temp.verts, temp.numVerts, temp.indices, temp.numIndices);
	return &geometries[lastGeo-1];
}

#pragma region Debug
void Renderer::debugBufferShapes(){
	Neumont::ShapeData temp = Neumont::ShapeGenerator::makeArrow();
	addGeometry(temp.verts, temp.numVerts, temp.indices, temp.numIndices);
	temp = Neumont::ShapeGenerator::makeCube();
	addGeometry(temp.verts, temp.numVerts, temp.indices, temp.numIndices);
	temp = Neumont::ShapeGenerator::makeSphere(10);
	addGeometry(temp.verts, temp.numVerts, temp.indices, temp.numIndices);
	temp = Neumont::ShapeGenerator::makeTorus(10);
	addGeometry(temp.verts, temp.numVerts, temp.indices, temp.numIndices);

	addRenderable(&geometries[0], glm::translate(glm::vec3(0,0,0)) * glm::scale(1.0f,1.0f,1.0f), programID[0]);
	addRenderable(&geometries[1], glm::translate(glm::vec3(4,0,-4)) * glm::scale(1.0f,1.0f,1.0f), programID[0]);
	addRenderable(&geometries[2], glm::translate(glm::vec3(5,0,5)) * glm::scale(1.0f,1.0f,1.0f), programID[0]);
	addRenderable(&geometries[3], glm::translate(glm::vec3(7,0,0)) * glm::scale(1.0f,1.0f,1.0f), programID[0]);

	addRenderable(&geometries[0], glm::translate(glm::vec3(0,5,0)) * glm::scale(1.0f,1.0f,1.0f), programID[1]);
	addRenderable(&geometries[1], glm::translate(glm::vec3(4,5,-4)) * glm::scale(1.0f,1.0f,1.0f), programID[1]);
	addRenderable(&geometries[2], glm::translate(glm::vec3(5,5,5)) * glm::scale(1.0f,1.0f,1.0f), programID[1]);
	addRenderable(&geometries[3], glm::translate(glm::vec3(7,5,0)) * glm::scale(1.0f,1.0f,1.0f), programID[1]);
}

void Renderer::readSomeAwesomeStuffFromAFile(){
	std::ifstream in("chestpiece.bin", std::ios::in | std::ios::binary);
	in.seekg(0, std::ios::end);
	const unsigned int BUFFERSIZE = in.tellg();
	char* buf = new char[BUFFERSIZE];
	in.seekg(0, std::ios::beg);
	in.read(buf, BUFFERSIZE);
	in.close();

	Neumont::ShapeData* shapeDatas = reinterpret_cast<Neumont::ShapeData*>(buf + sizeof(unsigned int));
	unsigned int numShapeDatas = (buf[0]);

	for (unsigned int i = 0; i < numShapeDatas; i++){
		Neumont::ShapeData& d = shapeDatas[i];
		d.verts = reinterpret_cast<Neumont::Vertex*>(buf + (unsigned int)d.verts);
		d.indices = reinterpret_cast<unsigned short*>(buf + (unsigned int)d.indices);
		addGeometry(shapeDatas[i].verts, shapeDatas[i].numVerts, shapeDatas[i].indices, shapeDatas[i].numIndices);
		Renderable* rend = addRenderable(&geometries[lastGeo-1], glm::translate(glm::vec3(15.0f, 0.0f, i*4.0f-10.0f)) * glm::scale(10.0f, 10.0f, 10.0f) * glm::rotate(-90.0f, glm::vec3(1.0f,0.0f,0.0f)), programID[2]);
	}
	delete [] buf;
}
#pragma endregion
#pragma region Shaders
uint Renderer::createShaders(const char* vertShaderName, const char* fragShaderName)
{
	uint vertID = glCreateShader(GL_VERTEX_SHADER);
	uint fragID = glCreateShader(GL_FRAGMENT_SHADER);
	programID[currentProgramID] = glCreateProgram();
	compileShader(vertShaderName, vertID);
	compileShader(fragShaderName, fragID);
	glAttachShader(programID[currentProgramID], vertID);
	glAttachShader(programID[currentProgramID], fragID);
	glLinkProgram(programID[currentProgramID]);
	++currentProgramID;
	return programID[currentProgramID-1];
}

void Renderer::removeRenderable(Renderable* removal){
	for (uint i=0;i<lastThing;i++){
		if (removal == &things[i]){
			for (uint j=i;j<lastThing-1;j++){
				things[j] = things[j+1];
			}
			lastThing--;
		}
	}
}

void Renderer::compileShader(const char* fileName, GLuint shaderID)  {
	QFile input(fileName);

	if( ! input.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		qDebug() << "File failed to open: " << fileName;
		exit(1);
	}

	QTextStream stream(&input);
	QString shaderCodeText = stream.readAll();
	input.close();

	//std::string tmp = shaderCodeText.toStdString();
	//const char* shaderSource = tmp.c_str();

	QByteArray ba = shaderCodeText.toLocal8Bit();
	const char* shaderSource = ba.data();

	glShaderSource(shaderID, 1, &shaderSource, NULL);
	glCompileShader(shaderID);

	GLint status;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE)
	{
		GLint infoLogLen;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLen);

		char* buffer = new char[infoLogLen];

		GLsizei BitBucket;

		glGetShaderInfoLog(shaderID, infoLogLen, &BitBucket, buffer);

		qDebug() << buffer;

		delete [] buffer;

		exit(1);
	}
}
#pragma endregion
void Renderer::paint(){
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);

	//Failed attempt at drawing a set of coordinates.
	/*glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	camera.getMatrix();
	glm::perspective(L->get<float>("camera.FOV"), ((float)width/height), L->get<float>("camera.nearPlane"), L->get<float>("camera.farPlane"));
	for (GLfloat i = -2.5; i <= 2.5; i += 0.25) {
		glVertex3f(i, 1, 2.5); glVertex3f(i, 0, -2.5);
		glVertex3f(2.5, 0, i); glVertex3f(-2.5, 0, i);
	}
	glEnd();*/

	for (uint i=0;i<lastThing;i++){
		things[i].paint(camera.getMatrix(), 
			glm::perspective(camera.FOV, ((float)width/height), camera.nearPlane, camera.farPlane));
	}

	for (uint i=0;i<points->size();i++){
		points->at(i).paint(camera.getMatrix(),
			glm::perspective(camera.FOV, ((float)width/height), camera.nearPlane, camera.farPlane));
		//qDebug() << "Paint";
	}
}

void Renderer::setWindowDims(uint wid, uint hei){
	width = wid;
	height = hei;
}

Geometry* Renderer::addGeometry(const void* verts, const uint numVerts, const void* indices, const uint numIndices){

	if (numVerts*sizeof(Neumont::Vertex) + vOffsets[currentBuffer] > 0x100000){
		++currentBuffer;
		glGenBuffers(1, &vBuffer[currentBuffer]);
		glGenBuffers(1, &iBuffer[currentBuffer]);
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer[currentBuffer]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer[currentBuffer]);

		glBufferData(GL_ARRAY_BUFFER, 0x100000, 0, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0x100000, 0, GL_STATIC_DRAW);
	}
	if (lastGeo < MAX_NUM_GEOMETRIES-1){
		geometries[lastGeo].setGeometry(verts, numVerts, indices, numIndices);
		geometries[lastGeo].putInBuffers(vBuffer[currentBuffer], iBuffer[currentBuffer], &vOffsets[currentBuffer], &iOffsets[currentBuffer]);
		return &geometries[lastGeo++];
	}else{
		return &geometries[lastGeo-1];
	}
}

Renderable* Renderer::addRenderable(const Geometry* geo, const glm::mat4& where, uint how, bool depth, uint drawMode, float life){
	if (lastThing < MAX_NUM_RENDERABLES-1){
		things[lastThing].init(geo, where, how, depth, drawMode);
		if (life != 0.0f){
			things[lastThing].setLife(life);
		}
		lastThing++;
		return &things[lastThing-1];
	}
}

glm::mat4 Renderer::align(const glm::vec3& originalOrientation, const glm::vec3& newOrientation)
{

	glm::vec3 original = glm::vec3(originalOrientation);
	glm::vec3 NewOrientation = glm::normalize(newOrientation);
	float yaw = acos(glm::dot(original, glm::normalize(glm::vec3(NewOrientation.x, 0, NewOrientation.z)))) * 180/PI;
	float pitch = 0.0f;
	if(yaw * 0 != 0)
	{
		yaw = 0;
		pitch = acos(glm::dot(original, glm::normalize(NewOrientation))) * 180/PI;
	}
	else 
	{
		if(NewOrientation.x < 0)
			yaw = -yaw;
		glm::vec3 pitchVector = glm::vec3(glm::vec4(original, 0) * glm::rotate(yaw, glm::vec3(0,1,0)));
		pitch = acos(glm::dot(pitchVector, glm::normalize(NewOrientation))) * 180/PI;
	}
	if(pitch * 0 != 0)
	{
		pitch = 0;
	}
	else if(NewOrientation.y > 0)
	{
		pitch = -pitch;
	}

	return glm::rotate(pitch, 1.0f,0.0f,0.0f) * glm::rotate(-yaw, 0.0f,1.0f,0.0f);

}

uint Renderer::addShader(const char* vertShaderFileName, const char* fragShaderFileName){
	uint vertID = glCreateShader(GL_VERTEX_SHADER);
	uint fragID = glCreateShader(GL_FRAGMENT_SHADER);
	uint progID = glCreateProgram();
	compileShader(vertShaderFileName, vertID);
	compileShader(fragShaderFileName, fragID);
	glAttachShader(progID, vertID);
	glAttachShader(progID, fragID);
	glLinkProgram(progID);
	return progID;
}

void Renderer::update(){
	float dt = timer.Interval();

	if (GetAsyncKeyState(0x57))
		camera.moveForward(dt);
	if (GetAsyncKeyState(0x53))
		camera.moveBackward(dt);
	if (GetAsyncKeyState(0x41))
		camera.strafeLeft(dt);
	if (GetAsyncKeyState(0x44))
		camera.strafeRight(dt);
	if (GetAsyncKeyState(0x20))
		camera.moveUp(dt);
	if (GetAsyncKeyState(0xA2))
		camera.moveDown(dt);
	if (GetAsyncKeyState(0x51))
		camera.turn(-dt*20);
	if (GetAsyncKeyState(0x45))
		camera.turn(dt*20);
	if (GetAsyncKeyState(0x46))
		camera.elevate(dt*10);
	if (GetAsyncKeyState(0x52))
		camera.elevate(-dt*10);
	if (GetAsyncKeyState(VK_ESCAPE))
		exit(0);
	//if (GetAsyncKeyState(0x4F))
	//	camera.changeSpeed(0.9f);
	//if (GetAsyncKeyState(0x50))
	//	camera.changeSpeed(1.1f);

	if (GetAsyncKeyState(0x49))
		points->at(0).position.x +=0.1f;
	if (GetAsyncKeyState(0x4A))
		points->at(0).position.z -= 0.1f;
	if (GetAsyncKeyState(0x4B))
		points->at(0).position.x -=0.1f;
	if (GetAsyncKeyState(0x4C))
		points->at(0).position.z += 0.1f;
	if (GetAsyncKeyState(0x55))
		points->at(0).position.y -=0.1f;
	if (GetAsyncKeyState(0x4F))
		points->at(0).position.y += 0.1f;
}