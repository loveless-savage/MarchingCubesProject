//#include "shLighting.h"
#include "Qtfringehead.hpp"
#include <QtWidgets/QApplication>


shared_ptr<OGLF::Model> external_cube;


int main(int argc, char *argv[]) {

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	// Jinta look here- this line gives a segfault
	external_cube = OGLF::LoadModel("./data/cube.obj");
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

	QApplication a(argc, argv);

	Qtfringehead mainWin;
	mainWin.show();

	return a.exec();


}
