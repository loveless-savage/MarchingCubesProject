//#include "shLighting.h"
#include "QtOGL.hpp"
#include <QtWidgets/QApplication>


 

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	
	QtOGL mainWin;
	mainWin.show();

    return a.exec();

	
}
