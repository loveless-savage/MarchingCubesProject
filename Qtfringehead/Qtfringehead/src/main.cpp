//#include "shLighting.h"
#include "Qtfringehead.hpp"
#include <QtWidgets/QApplication>


//shared_ptr<OGLF::Model> external_cube;


int main(int argc, char *argv[]) {

	QApplication a(argc, argv);

	Qtfringehead mainWin;
	mainWin.show();

	return a.exec();


}
