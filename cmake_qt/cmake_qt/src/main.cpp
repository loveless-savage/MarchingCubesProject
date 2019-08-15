#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWin, win2;

	// test main window with diagnostic messages
	mainWin.setMsg("heey");
	mainWin.showMsg();
	win2.setMsg("aw yiss");
	win2.showMsg();

    mainWin.show();

    return app.exec();
}
