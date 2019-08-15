#pragma once

#include "ShadowMapGL.hpp"
#include <QtWidgets/QMainWindow>
#include "ui_Qtfringehead.h"
/*
* =====================================================================================
*
*       Filename:
*
*    Description:  opengl framework
*
*        Version:  1.0
*        Created:  5/6/2018
*       Revision:  none
*       Compiler:  VS2013
*
*         Author:  JT
*
* =====================================================================================
*/

class Qtfringehead : public QMainWindow
{
	Q_OBJECT

public:
	Qtfringehead(QWidget *parent = Q_NULLPTR);

public:
	// ShadowMapGL is a child of OGLFramework, which is a child of QGLWidget
	// QGLWidget
	//  └──OGLF::OGLFramework
	//            └──ShadowMapGL
	ShadowMapGL* Fw;

private:
	Ui::QtfringeheadClass ui;
};
