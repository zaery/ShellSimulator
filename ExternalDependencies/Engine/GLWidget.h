#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <GL\glew.h>
#include <QtOpenGL\qglwidget>
#include <Qt\qtimer.h>
#include "ExportHeader.h"
#include "Renderer.h"
#include "ExportHeader.h"
#include <vector>
#include "GameObject.h"
#include "PhysicsEngine.h"
#include <memory>
class Geometry;
class Renderable;

class ENGINE_SHARED GLWidget : public QGLWidget
{
	Q_OBJECT
	shared_ptr<Renderable> lastRenderable;
	int lastX;
	int lastY;
	bool movingCamera;
	void initShapes();
	void keyPressEvent(QKeyEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void enterEvent(QEvent *e);
	void resizeEvent(QResizeEvent*);
	unique_ptr<QTimer> timer;

	shared_ptr<PhysicsEngine> phys;
	std::vector<PointCloud> points;

public:
	//Window
	std::vector<shared_ptr<Renderable>> nodeShapes;
	std::vector<shared_ptr<Renderable>> connectionShapes;
	std::vector<shared_ptr<GameObject>> gameObjects;
	std::vector<shared_ptr<Geometry>> geoms;
	Renderer renderer;
	void initializeGL();
	void paintGL();
	void initPlayers();
public slots:
	void update();
};
#endif