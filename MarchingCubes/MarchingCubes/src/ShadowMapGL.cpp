#include "ShadowMapGL.hpp"

extern double g_per_time_radio;
#define SHADOWMAP_RES_WIDTH 6024
#define SHADOWMAP_RES_HEIGHT 6024
bool anim_on = true;
bool spot_light_on = true;
bool direct_light_on = true;


// start up the whole thing
void ShadowMapGL::OnInit() {
// load shaders
	m_model_program = OGLF::CreateProgram(
		"./shaders/verts.c", "./shaders/frags.c");
	m_pc_model_program = OGLF::CreateProgram(
		"./shaders/purecolor_verts.c", "./shaders/purecolor_frags.c");
	// to render shadow texture from light's view point
	m_render_shadow_tex_program = OGLF::CreateProgram(
		"./shaders/render_shadow_tex_verts.c", "./shaders/render_shadow_tex_frags.c");

// load objects
	m_dragon_model = OGLF::LoadModel("./data/dragon.obj");
	// sphere for lights
	m_pc_model = OGLF::LoadModel("./data/sphere.obj");
	// plane object
	m_plane_model = OGLF::LoadModel("./data/plane.obj");

	// load lights
	light = Light(1, 1, 1, 0, 0, 0, Light_Type::DIRECTION);
	light2 = Light(1, 1, 1, 0, 0, 0, Light_Type::DIRECTION);

	// initial opengl states
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SMOOTH);
	// initialize framebuffer
	glGenFramebuffers(1, &m_framebuffer);

	// set up an interface between shadow generation and primary rendering
	glEnable(GL_TEXTURE_2D);
	this->initShadowMapTex(m_depth_tex, SHADOWMAP_RES_WIDTH, SHADOWMAP_RES_HEIGHT);
	this->initShadowMapTex(m_depth_tex2, SHADOWMAP_RES_WIDTH, SHADOWMAP_RES_HEIGHT);

	// projective matrix
	m_proj = glm::perspective(glm::radians(60.f), FrameRatio(), 0.1f, 100.f);
}


// run every time the scene is tweaked
void ShadowMapGL::OnUpdate() {
	// black background
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

	// rotate lights over time
	light.pos = glm::vec3(1, 5, 0);
	light.pos = glm::vec3(
		glm::rotate(float(720 * g_per_time_radio), glm::vec3(0, 1.0, 0))
		* vec4(light.pos, 1.0)
	);
	light2.pos = glm::vec3(8, 1, 0);
	light2.pos = glm::vec3(
		glm::rotate(float(45 * g_per_time_radio), glm::vec3(0, 1.0, 0))
		* vec4(light2.pos, 1.0)
	);

	// Shadow map 1: orthographic box projection
	// glm::ortho<T>(left, right, bottom, top, front, back)
	glm::mat4 depthProjectionMatrix =
		glm::ortho<float>(-10, 10, -10, 10, 0.0, 10);
	// matrix for transforming world to viewing coordinates
	// glm::lookAt(camera, target, up)
	glm::mat4 depthViewMatrix =
		glm::lookAt(light.pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	// this matrix figures out the first shadow
	depthMVP = depthProjectionMatrix * depthViewMatrix;

	// shadow map 2
	// glm::perspective<T>(fov_y, aspect_ratio, front, back)
	glm::mat4 depthProjectionMatrix2 =
		glm::ortho<float>(-10, 10, -10, 10, 0.0, 20);
		//glm::perspective<float>(glm::radians(40.0f), 1, 1.0, 20);
	// glm::lookAt(camera, target, up)
	glm::mat4 depthViewMatrix2 =
		glm::lookAt(light2.pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	//glm::mat4 depthModelMatrix = glm::mat4(1.0);
	depthMVP2 = depthProjectionMatrix2 * depthViewMatrix2;

// TODO
	std::vector<shared_ptr<OGLF::Model>> models;
	std::vector<glm::mat4> model_trans;

	// pop models onto the model "stack"
	models.push_back(m_dragon_model);
	// dragon  	
	glm::mat4 dragon_model_mat = m_model_trans * glm::scale(vec3(0.02));

	// pop model matrices onto the model matrix "stack"
	model_trans.push_back(dragon_model_mat);

	this->generateShadowMap(
		m_depth_tex,
		models,
		model_trans,
		depthMVP,
		SHADOWMAP_RES_WIDTH, SHADOWMAP_RES_HEIGHT);

	this->generateShadowMap(
		m_depth_tex2,
		models,
		model_trans,
		depthMVP2,
		SHADOWMAP_RES_WIDTH, SHADOWMAP_RES_HEIGHT);

	/**********************************************************************/
	//render phong objets
	/**********************************************************************/

	// redirect OpenGL actions to the default (screen) framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// reset dimensions to fit viewport
	glViewport(0, 0, this->width(), this->height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw dragon
	this->renderPhongObject(
		m_dragon_model,
		dragon_model_mat,
		glm::vec3(1., 1., 0.),
		glm::vec3(0.3, 0.6, 0.1),
		glm::vec3(1.0, 1.0, 1.0),
		glm::float32(6.0));

	// draw plane objets
	glm::mat4 plane_model_mat = glm::translate(glm::vec3(0, -1, 0)) * glm::scale(glm::vec3(0.02));
	this->renderPhongObject(
		m_plane_model,
		plane_model_mat,
		glm::vec3(1., 1., 1.),
		glm::vec3(0.3, 0.6, 0.1),
		glm::vec3(1.0, 1.0, 1.0),
		glm::float32(3.0));

	/**********************************************************************/
	//render pure color objects- lights
	/**********************************************************************/
	if (spot_light_on) {
		glm::mat4 lights_model_mat = glm::translate(light.pos) * glm::scale(vec3(0.1));
		this->renderPureObject(m_pc_model, lights_model_mat, light.color);
	}
	if (direct_light_on) {
		glm::mat4 lights_model_mat2 = glm::translate(light2.pos) * glm::scale(vec3(0.1));
		glm::vec3 obj_Color2 = glm::vec3(1.0, 1.0, 1.0);
		this->renderPureObject(m_pc_model, lights_model_mat2, light2.color);
	}
}


// render pure object
void ShadowMapGL::renderPureObject(
	shared_ptr<OGLF::Model>& pc_model,
	glm::mat4& model_mat,
	glm::vec3& obj_Color) {

	glUseProgram(m_pc_model_program);

	glm::mat4 model_view_proj = m_proj * m_camera_view * model_mat;

	// link 4x4 transformation matrices
	glUniformMatrix4fv(
		glGetUniformLocation(m_pc_model_program, "model_view_proj"),
		1, false, glm::value_ptr(model_view_proj));
	glUniformMatrix4fv(
		glGetUniformLocation(m_pc_model_program, "model"),
		1, false, glm::value_ptr(model_mat));
	// link array of 3 floats to transmit color
	glUniform3fv(
		glGetUniformLocation(m_pc_model_program, "uColor"),
		1, glm::value_ptr(obj_Color));

	// draw pure color objects
	pc_model->Draw(m_pc_model_program);

	glUseProgram(0);
}


// generate shadow map // TODO
void ShadowMapGL::generateShadowMap(
	GLuint& tex,
	const std::vector<shared_ptr<OGLF::Model>>& models,
	const std::vector<glm::mat4>& model_trans,
	const glm::mat4& depth_vp,
	uint width, uint height) {

	// render depth texture
	glUseProgram(m_render_shadow_tex_program);

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, tex, 0);
	glDrawBuffer(GL_NONE); // No color buffer is drawn to.
						   //glReadBuffer(GL_NONE);
						   // Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glViewport(0, 0, width, height);
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(
		glGetUniformLocation(m_render_shadow_tex_program, "depthMVP"),
		1, false, glm::value_ptr(depth_vp));

	for (int i = 0; i < models.size(); i++) {
		// Send our transformation to the currently bound shader,
		glUniformMatrix4fv(
			glGetUniformLocation(m_render_shadow_tex_program, "model"),
			1, false, glm::value_ptr(model_trans[i]));

		glCullFace(GL_FRONT);
		models[i]->Draw(m_render_shadow_tex_program);
		glCullFace(GL_BACK);
	}
}

// initialize texture map for shadow map
void ShadowMapGL::initShadowMapTex(GLuint& tex, uint width, uint height) {
	// initialize tex
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// render phong lighting
void ShadowMapGL::renderPhongObject(
	shared_ptr<OGLF::Model>& phong_model,
	glm::mat4& model_mat,
	glm::vec3 && obj_Color,
	glm::vec3 && uK,
	glm::vec3 && uSpecularColor,
	glm::float32 uShininess
) {

	glUseProgram(m_model_program);

	glm::mat4 model_view_proj = m_proj * m_camera_view  * model_mat;
	glm::mat4 normal_trans = glm::transpose(glm::inverse(model_mat));
	// delivery transforms
	glUniformMatrix4fv(
		glGetUniformLocation(m_model_program, "model_view_proj"),
		1, false, glm::value_ptr(model_view_proj));
	glUniformMatrix4fv(
		glGetUniformLocation(m_model_program, "model"),
		1, false, glm::value_ptr(model_mat));
	glUniformMatrix4fv(
		glGetUniformLocation(m_model_program, "normal_trans"),
		1, false, glm::value_ptr(normal_trans));
	// for light
	glUniformMatrix4fv(
		glGetUniformLocation(m_model_program, "depthMVP"),
		1, false, glm::value_ptr(depthMVP));

	glUniform3fv(
		glGetUniformLocation(m_model_program, "lightPosition"),
		1, glm::value_ptr(light.pos));
	glUniform1i(
		glGetUniformLocation(m_model_program, "light_type"), //SPOT 0,POINT 1,DIRECTION 2
		GLint(light.light_type));
	glUniform3fv(
		glGetUniformLocation(m_model_program, "lightColor"),
		1, glm::value_ptr(light.color));

	// for light2
	glUniformMatrix4fv(
		glGetUniformLocation(m_model_program, "depthMVP2"),
		1, false, glm::value_ptr(depthMVP2));

	glUniform3fv(
		glGetUniformLocation(m_model_program, "lightPosition2"),
		1, glm::value_ptr(light2.pos));
	glUniform1i(
		glGetUniformLocation(m_model_program, "light_type2"), //SPOT 0,POINT 1,DIRECTION 2
		GLint(light2.light_type));
	glUniform3fv(
		glGetUniformLocation(m_model_program, "lightColor2"),
		1, glm::value_ptr(light2.color));


	glUniform3fv(
		glGetUniformLocation(m_model_program, "uColor"),
		1, glm::value_ptr(obj_Color));
	glUniform3fv(
		glGetUniformLocation(m_model_program, "uSpecularColor"),
		1, glm::value_ptr(uSpecularColor));
	glUniform3fv(
		glGetUniformLocation(m_model_program, "uK"),
		1, glm::value_ptr(uK));

	glUniform1f(glGetUniformLocation(m_model_program, "uShininess"),
		uShininess);

	// Texture unit 0 is for shadowmap.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depth_tex);
	glUniform1i(glGetUniformLocation(m_model_program, "shadowMap"), 0);

	// Texture unit 1 is for shadowmap2.
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_depth_tex2);
	glUniform1i(glGetUniformLocation(m_model_program, "shadowMap2"), 1);

	phong_model->Draw(m_model_program);
	glUseProgram(0);
}

void ShadowMapGL::keyPressEvent(QKeyEvent *event) {
	// freeze/unfreeze render on press 'q'
	if (event->key() == Qt::Key_Q) {
		anim_on = !anim_on;
		if (!anim_on) {
			this->killTimer(m_timer_id);
		}
		else {
			m_timer_id = this->startTimer(0);
		}
	}
	// turn white light on/off with 'w'
	if (event->key() == Qt::Key_W) {
		spot_light_on = !spot_light_on;
		if (!spot_light_on) {
			light.color = glm::vec3(0.0);
		}
		else {
			light.color = glm::vec3(1.0);
		}
	}
	// turn red light on/off with 'e'
	if (event->key() == Qt::Key_E) {
		direct_light_on = !direct_light_on;
		if (!direct_light_on) {
			light2.color = glm::vec3(0.0);
		}
		else {
			light2.color = glm::vec3(1.0, 0.0, 0.0);
		}
	}

}
