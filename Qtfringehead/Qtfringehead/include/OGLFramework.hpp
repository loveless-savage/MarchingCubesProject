#ifndef OGLFRAMEWORK_H_
#define OGLFRAMEWORK_H_
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
#include <GL/glew.h>
#include <QGLWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>
#include <qtimer.h>

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

using glm::mat4;
using glm::vec3;

namespace OGLF
{


	// OGLFramework is a type of widget
	class OGLFramework : public QGLWidget
	{
		Q_OBJECT

	public:
		// main window with normal system frame
		OGLFramework(QWidget *parent = 0, Qt::WindowFlags flags = 0);

		// return the frame ratio
		inline float FrameRatio() { return float(this->width())/float(this->height()); }

	protected:
		// these two functions will be overwritten by the child class
		virtual void OnInit() {}
		virtual void OnUpdate() {}
	protected:
		/****QT_OpenGL_Framework**********************/
		void initializeGL(); // set up OpenGL in the display window
		void paintGL(); // wrapper function for OnUpdate()
		void resizeGL(int width, int height){ // OpenGL content size = window size
			// if window is flat
			if (0 == height) height = 1;
			// resize the viewport
			glViewport(0, 0, width, height);
		}
		void timerEvent(QTimerEvent* timer) override;
		/****interaction operation**********************/
		// scrolling is interpreted as zooming
		virtual void wheelEvent(QWheelEvent *e);
		// only move camera on drag when mouse is pressed (either button)
		virtual void mousePressEvent(QMouseEvent *e);
		// dragging mouse is interpreted as a trackball
		virtual void mouseMoveEvent(QMouseEvent *e);
		virtual void closeEvent(QCloseEvent *event)
		{
			this->hide();
		}

		bool lbutton_down_ = false;
		bool rbutton_down_ = false;

		/***model view matrix*************************/
		// camera's location in world space
		vec3 m_camera_position;
		// vector to roll the camera correctly
		vec3 m_camera_up;

		mat4 m_camera_view;
		mat4 m_model_trans;

		float lastx_ = 0.f, lasty_ = 0.f;

		int m_timer_id;
	private:
		//float pre_xpos_, pre_ypos_;

	};
}
#endif
