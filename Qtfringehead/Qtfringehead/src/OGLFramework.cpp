#define _USE_MATH_DEFINES
#define GLM_ENABLE_EXPERIMENTAL

#include "OGLFramework.hpp"

#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <qdatetime.h>

#include <stdexcept>
#include <string>
#include <functional>
#include <vector>
#include <iostream>
// sensitivity of mouse
#define MOUSE_ROTATED_PARAMETER 0.05
#define WHEEL_ROTATED_PARAMETER 0.01
// periodic time radio
#define TIME_PERIOD (1000 * 1000)
double g_per_time_radio;

using namespace OGLF;
using glm::vec3;
using glm::vec4;
using glm::mat4;

const double PI = float(M_PI);

// initialize OGLFramework
OGLFramework::OGLFramework(QWidget *parent, Qt::WindowFlags flags) : QGLWidget(parent) {
	this->setEnabled(true);
	m_model_trans = glm::mat4(1.0f);
	m_camera_position = { 0.0f, 0.0f, 8.0f };
	m_camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
	// get a matrix ready to shift world to view space
	m_camera_view = glm::lookAt(glm::vec3(m_camera_position), glm::vec3{ 0, 0, 0 }, m_camera_up);
	this->setFocusPolicy(Qt::StrongFocus);
	// set timer interval and start
	m_timer_id = this->startTimer(0);
}

// initialize the OpenGL
void OGLFramework::initializeGL() {
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{

		qDebug() << "glewInit error...................." << glewGetErrorString(err);
		return;
	}
	// fill system frame with OpenGL
	glViewport(0, 0, this->width(), this->height());
	this->OnInit();

}
// run on every update
void OGLFramework::paintGL() {
	this->OnUpdate();
}

void OGLFramework::timerEvent(QTimerEvent* timer) {
	// caculate the periodic time radio
	uint ms = QDateTime::currentMSecsSinceEpoch() - QDateTime(QDate::currentDate()).toMSecsSinceEpoch();	// milliseconds
	ms %= TIME_PERIOD;
	g_per_time_radio = ms * 1.0 / TIME_PERIOD;  // [ 0., 1. ) // TODO
	this->updateGL();
}

// interaction

// wheelEvent: scrolling is interpreted as zooming
void OGLFramework::wheelEvent(QWheelEvent *e) {

	QPoint numDegrees = e->angleDelta() / 8;
	float degree = numDegrees.y() * WHEEL_ROTATED_PARAMETER;
	//qDebug() << degree;
	// scale distance of camera from center based on scrolling
	m_camera_position += (m_camera_position - vec3(0)) * degree;
	m_camera_view = glm::lookAt(m_camera_position, glm::vec3(0), m_camera_up);

	this->updateGL();
}

// mousePressEvent: only move camera on drag when mouse is pressed (either button)
void OGLFramework::mousePressEvent(QMouseEvent *e) {
	// lastx_ & lasty_ normally store last frame position, but at first there is no previous frame to get position from
	lastx_ = e->x();
	lasty_ = e->y();
	if (e->button() == Qt::LeftButton) {
		lbutton_down_ = true;
		rbutton_down_ = false;
	} else if (e->button() == Qt::RightButton) {
		rbutton_down_ = true;
		lbutton_down_ = false;
	}
}

// mouseMoveEvent: dragging mouse when interpreted as a trackball
void OGLFramework::mouseMoveEvent(QMouseEvent *e) {
	// x & y are macros for e->x() and e->y()
	float x = e->x();
	float y = e->y();

	// set up instance variables
	float _r = 1.0;
	vec3 center(0.0, 0.0, 0.0);
	float past_x, past_y, past_z;
	float curr_x, curr_y, curr_z;
// convert last mouse pos into 3D coordinates on surface of a spherical trackball
	// normalize the mouse coordinate based on the center of screen
	past_x = (lastx_ - 0.5 * this->width() ) / (0.5 * this->width());
	past_y = (0.5 * this->height() - lasty_ ) / (0.5 * this->height());
	// square length of (past_x, past_y)
	float xxyy = (past_x*past_x + past_y*past_y);
	if (xxyy > 1) xxyy = 1; // length normalized! if(xxyy > 1) mouse is offscreen
	// assuming spherical trackball, extrapolate z_ dimension from x_ & y_
	past_z = sqrt(_r - xxyy);
	vec3 past_position(past_x, past_y, past_z);
// do it all again with current mouse pos
	curr_x = (x - 0.5 * this->width() ) / (0.5 * this->width());
	curr_y = (0.5 * this->height() - y ) / (0.5 * this->height());
	xxyy = (curr_x*curr_x + curr_y*curr_y);
	if (xxyy > 1) xxyy = 1;
	curr_z = sqrt(_r - xxyy);
	vec3 curr_position(curr_x, curr_y, curr_z);

	// define rotation plane with normal axis
	vec3 rot_axis = cross(past_position, curr_position);

	// calculate angle
	float cos_theta = dot(past_position, curr_position) / (length(past_position) * length(curr_position));
	float radian = acos(cos_theta);
	float angle = radian / PI * 180.0;
	// bake all those numbers into a rotation matrix
	mat4 trans_ = rotate(float(angle * MOUSE_ROTATED_PARAMETER), rot_axis);

	// if right button, drag camera
	if (rbutton_down_) {

		float dx = (x - lastx_)*0.003f;
		float dy = -(y - lasty_)*0.003f;
		auto dir = glm::normalize(m_camera_position); // vec3
		auto right = normalize(cross(dir, m_camera_up)); // vec3
		auto hrot = glm::rotate(dx, m_camera_up); // mat4
		auto vrot = glm::rotate(dy, right); // mat4
		auto newpos = hrot*vrot*glm::vec4{ m_camera_position, 1 }; // vec4
		auto newdir = glm::normalize(glm::vec3(newpos)); // vec3
		if (abs(glm::dot(newdir, m_camera_up)) < 0.99f) {
			m_camera_position = glm::vec3(newpos); // vec3
			m_camera_view = glm::lookAt(glm::vec3(newpos), glm::vec3{ 0, 0, 0 }, m_camera_up); // vec3
		}
	}

	// if left button, drag model
	if (lbutton_down_) {
		m_model_trans = trans_ * m_model_trans;
	}

	// tap OpenGL on the shoulder to let it know you made some changes
	this->updateGL();

	// store lastx_ and lasty_ to measure change from last frame
	lastx_ = x;
	lasty_ = y;
}
