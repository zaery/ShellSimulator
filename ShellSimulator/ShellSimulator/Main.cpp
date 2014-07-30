#include <Qt\qapplication.h>
#include "ShellSimulationWindow.h"

int main(int argc, char* argv[]){
	QApplication app(argc, argv);
	ShellSimulationWindow window;
	window.init();
	window.show();
	return app.exec();
}