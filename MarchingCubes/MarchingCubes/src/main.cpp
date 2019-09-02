//#include "shLighting.h"
#include "QtWrapper.hpp"
#include <QtWidgets/QApplication>




int main(int argc, char *argv[]){
	QApplication a(argc, argv);

	QtWrapper mainWin;
	mainWin.show();

	return a.exec();


}
