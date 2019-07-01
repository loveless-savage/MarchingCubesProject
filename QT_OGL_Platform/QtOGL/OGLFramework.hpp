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


	class OGLFramework : public QGLWidget
	{
		Q_OBJECT

	public:
		OGLFramework(QWidget *parent = 0, Qt::WindowFlags flags = 0);
		inline float FrameRatio()
		{
			return float(this->width()) / float(this->height());
		}

	protected:
		virtual void OnInit(){}
		virtual void OnUpdate(){}
	protected:
		/****QT_OpenGL_Framework**********************/
		void initializeGL();
		void paintGL();
		void resizeGL(int width, int height)
		{
			if (0 == height)
			{
				height = 1;
			}
			glViewport(0, 0, width, height);
		}
		void timerEvent(QTimerEvent* timer) override;
		/****interaction operation**********************/
		virtual void wheelEvent(QWheelEvent *e);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void mouseMoveEvent(QMouseEvent *e);
		virtual void closeEvent(QCloseEvent *event)
		{
			this->hide();
		}
	
        bool lbutton_down_ = false;
		bool rbutton_down_ = false;

		/***model view matrix*************************/
		vec3 m_camera_position;
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