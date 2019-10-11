//#include "shLighting.h"
#include "QtWrapper.hpp"
#include <QtWidgets/QApplication>




int main(int argc, char *argv[]){
	QApplication a(argc, argv);

	QtWrapper mainWin(6.f);
	mainWin.show();

	return a.exec();


}
