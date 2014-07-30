#pragma once
#include <Qt\qwidget.h>
#include "Renderer.h"
#include "GLWidget.h"
#include <memory>
class ShellSimulationWindow : public QWidget
{
	Q_OBJECT
	unique_ptr<GLWidget> glWid;
public:
	void init();
};