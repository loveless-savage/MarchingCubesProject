#include "Qtfringehead.hpp"

using namespace OGLF;
#define WINDOW_SIZE_WIDTH 800
#define WINDOW_SIZE_HEIGHT 600

Qtfringehead::Qtfringehead(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setGeometry(QRect(50, 100, WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT));
	/*class you define*/
	this->Fw = new ShadowMapGL();
	/*****************/
	this->setCentralWidget(Fw);
}
