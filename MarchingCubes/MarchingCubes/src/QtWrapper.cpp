#include "QtWrapper.hpp"

using namespace OGLF;
#define WINDOW_SIZE_WIDTH 800
#define WINDOW_SIZE_HEIGHT 600

QtWrapper::QtWrapper(float frameSize, QWidget *parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
	this->setGeometry(QRect(50, 100, WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT));
	// setup marching cubes class
	this->MCModel_table = new CubeTable();
	this->MCModel = new MarchingCubes(frameSize, MCModel_table);

	// setup opengl wrapper for user-defined functionality
	this->Fw = new ShadowMapGL();
	this->Fw->giveMarchingCubes_model(MCModel);
	/*****************/
	this->setCentralWidget(Fw);
}

// default frame size is [-10.f, 10.f] in all dimensions
QtWrapper::QtWrapper(QWidget *parent): QtWrapper(10.0f, parent){}
