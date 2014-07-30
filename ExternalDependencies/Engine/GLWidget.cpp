#include "GLWidget.h"
#include <iostream>
#include <Qt\qevent.h>
#include "Geometry.h"
#include <ShapeData.h>
#include <ShapeGenerator.h>
#include <fstream>
#include <Qt\qdebug.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include "LuaState.h"
#include <memory>

void GLWidget::initializeGL(){
	glewInit();
	show();
	grabKeyboard();
	setMouseTracking(true);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//LUA
	{
		unique_ptr<LuaState> L = unique_ptr<LuaState>(new LuaState("config.lua"));
		glClearColor(L->get<float>("render.clearColorR"),L->get<float>("render.clearColorG"),L->get<float>("render.clearColorB"),L->get<float>("render.clearColorA"));
	}
	movingCamera = false;
	points.push_back(PointCloud());
	points.push_back(PointCloud(3.1f));
	renderer.init(&points);
	//debugManager.init(&renderer);
	//initShapes();

	std::vector<PhysicsComponent*> physicsComponents;
	for (int i=0;i<gameObjects.size();i++){
		//gameObjects[i]->getComponent<PhysicsComponent>();
	}

	phys = shared_ptr<PhysicsEngine>(new PhysicsEngine(&points));

	timer = unique_ptr<QTimer>(new QTimer(this));
	connect(timer.get(), SIGNAL(timeout()), this, SLOT(update()));
	timer->start(0);
}

void GLWidget::update(){
	phys->update(0.016666667f);
	for (int i=0;i < gameObjects.size();i++){
		gameObjects[i]->update(0.016666667f);
	}
	renderer.update();
	//debugMenu->update();
	repaint();
}

void GLWidget::initShapes(){
#pragma region debug
	//Neumont::ShapeData temp = Neumont::ShapeGenerator::makeArrow();
	//geoms.push_back(renderer.addGeometry(temp.verts, temp.numVerts, temp.indices, temp.numIndices));
	//renderer.addRenderable(geoms[0], glm::mat4(), 0);
	//temp = Neumont::ShapeGenerator::makeCube();
	//geoms[1] = renderer.addGeometry(temp.verts, temp.numVerts, temp.indices, temp.numIndices);
	//temp = Neumont::ShapeGenerator::makeTorus(10);
	//geoms[2] = renderer.addGeometry(temp.verts, temp.numVerts, temp.indices, temp.numIndices);
	//temp = Neumont::ShapeGenerator::makeSphere(10);
	//geoms[3] = renderer.addGeometry(temp.verts, temp.numVerts, temp.indices, temp.numIndices);
#pragma endregion
}

void GLWidget::paintGL(){
	renderer.paint();
}

void GLWidget::keyPressEvent(QKeyEvent *e){
	int key = e->key();
#pragma region debugShapes
	//if (key==Qt::Key_U){
	//	debugManager.AddCross(renderer.camera.position + 2.0f*renderer.camera.lookDir, 1.0f, &lastRenderable, 0.0f);
	//}
	//if (key==Qt::Key_I){
	//	debugManager.AddSphere(renderer.camera.position + 2.0f*renderer.camera.lookDir, 1.0f, &lastRenderable, 0.0f);
	//}
	//if (key==Qt::Key_Y){
	//	debugManager.AddBoundingBox(lastRenderable, &lastRenderable, 0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
	//}
	//if (key==Qt::Key_T){
	//	debugManager.AddLine(renderer.camera.position, glm::vec3(), &lastRenderable, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f,1.0f), true);
	//}
	//if (key==Qt::Key_G){
	//	debugManager.AddAxes(lastRenderable, &lastRenderable, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
	//}
#pragma endregion
	if (key==Qt::Key_QuoteLeft || Qt::Key_AsciiTilde){
		//debugMenu->toggle();
	}
}

void GLWidget::initPlayers(){
	//Neumont::ShapeData arrow = Neumont::ShapeGenerator::makeArrow();
	//Neumont::ShapeData arrow = Neumont::ShapeGenerator::makeSphere(20);
	//unique_ptr<Geometry> arrowGeom = unique_ptr<Geometry>(renderer.addGeometry(arrow.verts, arrow.numVerts, arrow.indices, arrow.numIndices));

	//gameObjects.push_back(shared_ptr<GameObject>(new GameObject(glm::vec3(25,5,-25),renderer.addRenderable(arrowGeom.get(), glm::translate(glm::vec3(25,5,-25)), renderer.programID[4]),
	//	&renderer)));
	//gameObjects[gameObjects.size()-1]->addComponent(
	//	new PathFindingComponent(gameObjects.at(gameObjects.size()-1).get(), &renderer, &(nodes.at(0)), 
	//	nodes.size(), &(connections.at(0)), connections.size()));

	//gameObjects.push_back(shared_ptr<GameObject>(new GameObject(glm::vec3(-25,5,25),renderer.addRenderable(arrowGeom.get(), glm::translate(glm::vec3(-25,5,25)), renderer.programID[4]),
	//	&renderer)));
	//gameObjects[gameObjects.size()-1]->addComponent(new PathFindingComponent(gameObjects.at(gameObjects.size()-1).get(), &renderer, &(nodes.at(0)), nodes.size(), &(connections.at(0)), connections.size()));
}

void GLWidget::mouseMoveEvent(QMouseEvent *e){
	if (movingCamera){
		int x = e->x();
		int y = e->y();
		if (x >0)
			renderer.camera.turn(x-lastX);
		if (y>0)
			renderer.camera.elevate(y-lastY);
		lastX = (x);
		lastY = (y);
		glDraw();
	}
}

void GLWidget::mousePressEvent(QMouseEvent *e){
	movingCamera = true;
	int x = e->x();
	int y = e->y();
	lastX = (x);
	lastY = (y);
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e){
	e;
	movingCamera = false;
}

void GLWidget::enterEvent(QEvent *e){
	e;
	lastX = (QWidget::mapFromGlobal(QCursor::pos()).x());
	lastY = (QWidget::mapFromGlobal(QCursor::pos()).y());
	glDraw();
}

void GLWidget::resizeEvent(QResizeEvent* e){
	e;
	renderer.setWindowDims(width(), height());
	glViewport(0,0,width(), height());
}

#include "moc_GLWidget.cpp"