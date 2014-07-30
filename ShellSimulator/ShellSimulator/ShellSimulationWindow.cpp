#include "ShellSimulationWindow.h"
#include <QtGui\qvboxlayout>
#include "PointCloud.h"
void ShellSimulationWindow::init(){
	QVBoxLayout* mainLayout;
	setLayout(mainLayout = new QVBoxLayout);
	glWid = unique_ptr<GLWidget>(new GLWidget());
	mainLayout->addWidget(glWid.get(), 1);
	glWid->setMinimumSize(800,600);
}