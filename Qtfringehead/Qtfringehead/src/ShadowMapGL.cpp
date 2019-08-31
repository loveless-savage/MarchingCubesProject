#include "ShadowMapGL.hpp"

extern double g_per_time_radio;
#define SHADOWMAP_RES_WIDTH 6024
#define SHADOWMAP_RES_HEIGHT 6024
bool anim_on = true;
bool spot_light_on = true;
bool direct_light_on = true;


// start up the whole thing
void ShadowMapGL::OnInit() {
	//load shaders******************************/
	m_phong_model_program = OGLF::CreateProgram(
		"./shaders/verts.c", "./shaders/frags.c");
	m_purecolor_model_program = OGLF::CreateProgram(
		"./shaders/purecolor_verts.c", "./shaders/purecolor_frags.c");
	m_render_shadow_tex_program = OGLF::CreateProgram(
		"./shaders/render_shadow_tex_verts.c", "./shaders/render_shadow_tex_frags.c");
	m_render_mirror_tex_program = OGLF::CreateProgram(
		"./shaders/render_mirror_tex_verts.c", "./shaders/render_mirror_tex_frags.c");
	m_mirror_model_program = OGLF::CreateProgram(
		"./shaders/mirror_verts.c", "./shaders/mirror_frags.c");

	// load objects***************************/
	m_dragon_model = OGLF::LoadModel("./data/dragon.obj");
	m_dragon_model->addColorBuffer(3); // give it confetti colors!

	m_cube_model = OGLF::LoadModel("./data/cube.obj");
	m_cube_model->addColorBuffer(3); // give it confetti colors!

	// load model and pure color 
	m_pc_model = OGLF::LoadModel("./data/sphere.obj");
	// load plane objects
	m_plane_model = OGLF::LoadModel("./data/plane.obj");

	//load lights*****************************/
	// light and light view matrix
	light = Light(1, 1, 1, 2, 3, 0, Light_Type::DIRECTION);
	light2 = Light(1, 0, 0, 3, 5, 0, Light_Type::SPOT);

	// initial opengl states
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SMOOTH);

	// set up an interface between shadow generation and primary rendering
	glEnable(GL_TEXTURE_2D);
	this->initShadowMapTex(m_shadow_tex, SHADOWMAP_RES_WIDTH, SHADOWMAP_RES_HEIGHT);
	this->initShadowMapTex(m_shadow_tex2, SHADOWMAP_RES_WIDTH, SHADOWMAP_RES_HEIGHT);
	// interface between mirror rendering and primary rendering
	this->initMirrorTex(m_mirror_tex, this->width(), this->height());

	// projective matrix
	m_proj = glm::perspective(glm::radians(60.f), FrameRatio(), 0.1f, 100.f);
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// run every time the scene is tweaked
void ShadowMapGL::OnUpdate() {
	// model "stack", a dynamic array of memory-managed pointers to all models
	std::vector<shared_ptr<OGLF::Model>> models;
	// pop models onto the model "stack"
	models.push_back(m_dragon_model);
	models.push_back(m_cube_model);
	// calculate model matrices
	dragon_model_mat = glm::translate(vec3(2.0, 1.0, 0.0)) * m_model_trans * glm::scale(vec3(0.02));
	cube_model_mat = glm::rotate(float(90 * g_per_time_radio), glm::vec3(0, 1.0, 0))
		* glm::translate(vec3(0.0, 0.0, 0.0));
	// stack of model transformation matrices
	std::vector<glm::mat4> model_trans;
	// pop model matrices onto the model matrix "stack"
	model_trans.push_back(dragon_model_mat);
	model_trans.push_back(cube_model_mat);

	// activate lights and rotate them over time
	light.pos = glm::vec3(2, 3, 0);
	light.pos = glm::vec3(
		glm::rotate(float(360 * g_per_time_radio), glm::vec3(0, 1.0, 0))
		* vec4(light.pos, 1.0)
	);
	// notice that this light rotates twice as fast
	light2.pos = glm::vec3(3, 5, 0);
	light2.pos = glm::vec3(
		glm::rotate(float(720 * g_per_time_radio), glm::vec3(0, 1.0, 0))
		* vec4(light.pos, 1.0)
	);

	// shadow map view matrices
	// Shadow map 1: orthographic box projection
	// glm::ortho<T>(left, right, bottom, top, front, back)
	glm::mat4 shadowProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, 0.0, 10);
	// treat light as a camera for purpose of shadow rendering
	// glm::lookAt(camera, target, up)
	glm::mat4 shadowViewMatrix = glm::lookAt(light.pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	// this matrix figures out the first shadow
	shadowMVP = shadowProjectionMatrix * shadowViewMatrix;
	// shadow map 2
	// glm::perspective<T>(fov_y, aspect_ratio, front, back)
	glm::mat4 shadowProjectionMatrix2 = glm::perspective<float>(glm::radians(40.0f), 1, 1.0, 20);
	// glm::lookAt(camera, target, up)
	glm::mat4 shadowViewMatrix2 = glm::lookAt(light2.pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	//glm::mat4 shadowModelMatrix = glm::mat4(1.0);
	shadowMVP2 = shadowProjectionMatrix2 * shadowViewMatrix2;

	// draw shadow maps
	this->generateShadowMap(
		m_shadow_tex,
		models,
		model_trans,
		shadowMVP,
		SHADOWMAP_RES_WIDTH, SHADOWMAP_RES_HEIGHT
	);
	this->generateShadowMap(
		m_shadow_tex2,
		models,
		model_trans,
		shadowMVP2,
		SHADOWMAP_RES_WIDTH, SHADOWMAP_RES_HEIGHT
	);

	// black background
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

	// redirect OpenGL actions to the default (screen) framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// reset dimensions to fit viewport
	glViewport(0, 0, this->width(), this->height());

	// wipe depth buffer since last frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw mirror
	this->renderMirror(
		m_mirror_tex,
		m_plane_model,
		this->width(),this->height()
	);

	// all the object rendering is wrapped in here so we can reuse it for mirrors
	this->drawAllTheStuff();
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// all the objects rendered with desired settings in here
void ShadowMapGL::drawAllTheStuff(bool inMirror){
	//draw dragon
	this->renderPhongObject(
		m_dragon_model,
		dragon_model_mat,
		glm::vec3(0.3, 0.6, 0.1),
		glm::vec3(1.0, 1.0, 1.0),
		glm::float32(6.0),
		inMirror,
		true);

	//draw cube
	this->renderPhongObject(
		m_cube_model,
		cube_model_mat,
		glm::vec3(0.3, 0.6, 0.1),
		glm::vec3(1.0, 1.0, 1.0),
		glm::float32(6.0),
		inMirror,
		false, glm::vec3(1.0, 0.0, 0.0));

	// render pure color objects- lights
	if (spot_light_on) {
		glm::mat4 lights_model_mat = glm::translate(light.pos) * glm::scale(vec3(0.1));
		this->renderPureObject(m_pc_model, lights_model_mat, light.color);
	}
	if (direct_light_on) {
		glm::mat4 lights_model_mat2 = glm::translate(light2.pos) * glm::scale(vec3(0.1));
		this->renderPureObject(m_pc_model, lights_model_mat2, light2.color);
	}
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// generate shadow map
void ShadowMapGL::generateShadowMap(
	GLuint& tex,
	const std::vector<shared_ptr<OGLF::Model>>& models,
	const std::vector<glm::mat4>& model_trans,
	const glm::mat4& shadow_vp,
	uint width, uint height
) {

	// render shadow texture
	glUseProgram(m_render_shadow_tex_program);

	// don't render the shadows directly to the screen!
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_fb);
	// reroute rendering from framebuffer to a texture for future access
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
	// No color buffer is drawn to
	glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_fb);
	glViewport(0, 0, width, height); // match framebuffer dimensions to window size
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT); // clear the depth buffer

	// light source view matrix for all models to access in common
	glUniformMatrix4fv(
		glGetUniformLocation(m_render_shadow_tex_program, "shadowMVP"),
		1, false, glm::value_ptr(shadow_vp));

	// process all models in turn separately
	for (int i = 0; i < models.size(); i++) {
		// send model transformations to the shader
		glUniformMatrix4fv(
			glGetUniformLocation(m_render_shadow_tex_program, "model"),
			1, false, glm::value_ptr(model_trans[i]));

		// shadows affect facing away from light, so cut out front-facing primitives
		glCullFace(GL_FRONT);
		// do all the shadow math with attached shaders
		models[i]->Draw(m_render_shadow_tex_program);
		// switch culling to back faces again to prepare for normal rendering
		glCullFace(GL_BACK);
	}
}
// initial setup for shadow texture map
void ShadowMapGL::initShadowMapTex(GLuint& tex, uint width, uint height) {
	// initialize framebuffer for shadow rendering
	glGenFramebuffers(1, &m_shadow_fb);

	// initialize tex
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	// no texel blending when scaling up or down
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// boundary behavior if shadowmap is too small
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// luminance (monochrome) rather than rgb
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	// write null to the entire texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}


// render mirror
void ShadowMapGL::renderMirror(
	GLuint& tex,
	const shared_ptr<OGLF::Model>& mirror_model,
	//const std::vector<glm::mat4>& model_trans,
	uint width, uint height
) {

// step 1: generate texture
	// don't render the mirror action directly to the screen!
	glBindFramebuffer(GL_FRAMEBUFFER, m_mirror_fb);
	// reroute rendering from framebuffer to the texture for future access
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glViewport(0, 0, width, height); // match framebuffer dimensions to window size
	glClearDepth(1.0); // max distance
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the color and depth buffers

	// where is the mirror plane?
	glm::vec3 plane_transl(0, -0.5, 0);
	glm::mat4 plane_model_mat = glm::translate(plane_transl) * glm::scale(glm::vec3(0.02));

	// move the camera to capture the mirror's viewpoint
	glm::vec3 camera_transl = 2.f*glm::proj(m_camera_position-plane_transl, plane_transl); // weak
	m_camera_view = glm::lookAt(
		// camera position
		glm::vec3(m_camera_position)-camera_transl,
		// look at center
		glm::vec3{ 0, 0, 0 }-2.f*plane_transl,
		// camera upside down
		-1.f*m_camera_up
	);

	// render all the objects in here
	this->drawAllTheStuff(true);

	// restore original camera position
	m_camera_view = glm::lookAt(glm::vec3(m_camera_position), glm::vec3{ 0, 0, 0 }, m_camera_up);


// step 2: render mirror with texture attached
	// redirect OpenGL actions to the default (screen) framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// reset dimensions to fit viewport
	glViewport(0, 0, this->width(), this->height());

	// draw plane object
	glUseProgram(m_mirror_model_program);

	glm::mat4 model_view_proj = m_proj * m_camera_view * plane_model_mat;
	// link 4x4 transformation matrices
	glUniformMatrix4fv(
		glGetUniformLocation(m_mirror_model_program, "model_view_proj"),
		1, false, glm::value_ptr(model_view_proj));
	glUniform3fv(
		glGetUniformLocation(m_mirror_model_program, "mirror_pos"),
		1, glm::value_ptr(plane_transl));

	// pass viewport width/height to normalize gl_FragCoord
	glUniform1i(
		glGetUniformLocation(m_mirror_model_program, "width"),
		this->width());
	glUniform1i(
		glGetUniformLocation(m_mirror_model_program, "height"),
		this->height());

	// bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(
		glGetUniformLocation(m_mirror_model_program, "mirrorTexture"),
		0);

	// draw the mirror
	mirror_model->Draw(m_mirror_model_program);

	glUseProgram(0);
}
// initial setup for mirror texture map
void ShadowMapGL::initMirrorTex(GLuint& tex, uint width, uint height) {
	// framebuffer for mirror rendering
	glGenFramebuffers(1, &m_mirror_fb);
	glBindFramebuffer(GL_FRAMEBUFFER, m_mirror_fb);
	// add renderbuffer object, to manage depth and stencil // TODO
	glGenRenderbuffers(1, &m_mirror_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_mirror_rbo);
	// our renderbuffer will have depth and stencil testing
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	// connect the renderbuffer to the framebuffer
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_mirror_rbo);

	// initialize tex
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	// no texel blending when scaling up or down
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// boundary behavior if shadowmap is too small
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// rgb
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_RGB);
	// write null to the entire texture (unsigned byte correct?)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//render pure object
void ShadowMapGL::renderPureObject(
	shared_ptr<OGLF::Model>& pc_model,
	glm::mat4& model_mat,
	glm::vec3& obj_Color) {

	glUseProgram(m_purecolor_model_program);

	glm::mat4 model_view_proj = m_proj * m_camera_view * model_mat;

	// link 4x4 transformation matrices
	glUniformMatrix4fv(
		glGetUniformLocation(m_purecolor_model_program, "model_view_proj"),
		1, false, glm::value_ptr(model_view_proj));
	glUniformMatrix4fv(
		glGetUniformLocation(m_purecolor_model_program, "model"),
		1, false, glm::value_ptr(model_mat));
	// link array of 3 floats to transmit color
	glUniform3fv(
		glGetUniformLocation(m_purecolor_model_program, "uColor"),
		1, glm::value_ptr(obj_Color));

	//draw pure color objects
	pc_model->Draw(m_purecolor_model_program);

	glUseProgram(0);
}

//render phong lighting
void ShadowMapGL::renderPhongObject(
	shared_ptr<OGLF::Model>& phong_model,
	glm::mat4& model_mat,
	glm::vec3 && uK,
	glm::vec3 && uSpecularColor,
	glm::float32 uShininess,
	bool inMirror,
	bool isColorful,
	glm::vec3 && obj_Color
) {

	GLuint* m_current_model_program;
	if(inMirror){
		m_current_model_program = &m_render_mirror_tex_program;
	}else{
		m_current_model_program = &m_phong_model_program;
	}
	glUseProgram(*m_current_model_program);

	glm::mat4 model_view_proj = m_proj * m_camera_view  * model_mat;
	glm::mat4 normal_trans = glm::transpose(glm::inverse(model_mat));
	// model transformation matrices
	glUniformMatrix4fv(
		glGetUniformLocation(*m_current_model_program, "model_view_proj"),
		1, false, glm::value_ptr(model_view_proj));
	glUniformMatrix4fv(
		glGetUniformLocation(*m_current_model_program, "model"),
		1, false, glm::value_ptr(model_mat));
	glUniformMatrix4fv(
		glGetUniformLocation(*m_current_model_program, "normal_trans"),
		1, false, glm::value_ptr(normal_trans));

	// for light
	glUniformMatrix4fv(
		glGetUniformLocation(*m_current_model_program, "shadowMVP"),
		1, false, glm::value_ptr(shadowMVP));
	glUniform3fv(
		glGetUniformLocation(*m_current_model_program, "lightPosition"),
		1, glm::value_ptr(light.pos));
	glUniform1i(
		glGetUniformLocation(*m_current_model_program, "light_type"), //SPOT 0,POINT 1,DIRECTION 2
		GLint(light.light_type));
	glUniform3fv(
		glGetUniformLocation(*m_current_model_program, "lightColor"),
		1, glm::value_ptr(light.color));

	// for light2
	glUniformMatrix4fv(
		glGetUniformLocation(*m_current_model_program, "shadowMVP2"),
		1, false, glm::value_ptr(shadowMVP2));
	glUniform3fv(
		glGetUniformLocation(*m_current_model_program, "lightPosition2"),
		1, glm::value_ptr(light2.pos));
	glUniform1i(
		glGetUniformLocation(*m_current_model_program, "light_type2"), //SPOT 0,POINT 1,DIRECTION 2
		GLint(light2.light_type));
	glUniform3fv(
		glGetUniformLocation(*m_current_model_program, "lightColor2"),
		1, glm::value_ptr(light2.color));

	// colors
	glUniform1i(glGetUniformLocation(*m_current_model_program, "isColorful"),
		isColorful);
	glUniform3fv(
		glGetUniformLocation(*m_current_model_program, "uColor"),
		1, glm::value_ptr(obj_Color));
	glUniform3fv(
		glGetUniformLocation(*m_current_model_program, "uSpecularColor"),
		1, glm::value_ptr(uSpecularColor));
	glUniform3fv(
		glGetUniformLocation(*m_current_model_program, "uK"),
		1, glm::value_ptr(uK));

	glUniform1f(glGetUniformLocation(*m_current_model_program, "uShininess"),
		uShininess);

	// Texture unit 0 is for shadowmap
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_shadow_tex);
	// looks like a single integer uniform, but actually a sampler uniform
	glUniform1i(glGetUniformLocation(*m_current_model_program, "shadowMap"), 0);

	// Texture unit 1 is for shadowmap2
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_shadow_tex2);
	glUniform1i(glGetUniformLocation(*m_current_model_program, "shadowMap2"), 1);

	phong_model->Draw(*m_current_model_program);
	glUseProgram(0);
}


// key press actions
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

