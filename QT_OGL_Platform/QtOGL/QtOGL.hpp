#pragma once

#include "ShadowMapGL.hpp"
#include <QtWidgets/QMainWindow>
#include "ui_QtOGL.h"
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

class QtOGL : public QMainWindow
{
    Q_OBJECT

public:
    QtOGL(QWidget *parent = Q_NULLPTR);

public:
	ShadowMapGL* Fw;

private:
    Ui::QtOGLClass ui;
};
