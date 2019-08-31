#ifndef SHADOWMAPGL_H_
#define SHADOWMAPGL_H_

#define GLM_ENABLE_EXPERIMENTAL
#include "type.hpp"
#include "OGLFramework.hpp"
#include "loadingModel.hpp"
#include "loadingShaders.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/projection.hpp>
#include <iostream>

// ShadowMapGL is a child of OGLFramework, which is a child of QGLWidget
// QWidget
// 	└──QGLWidget
// 	    └──OGLF::OGLFramework
// 	              └──ShadowMapGL
class ShadowMapGL : public OGLF::OGLFramework {

public:
	ShadowMapGL() {}
	~ShadowMapGL() {
		glDeleteProgram(m_phong_model_program);
		glDeleteProgram(m_purecolor_model_program);
		glDeleteProgram(m_render_shadow_tex_program);
		glDeleteProgram(m_mirror_model_program);
	}

private:

	// models
	shared_ptr<OGLF::Model> m_dragon_model, m_cube_model;
	glm::mat4 dragon_model_mat, cube_model_mat;
	// container for the shader program
	GLuint m_phong_model_program;
	// view (global->camera) and projective (camera->perspective) matrices
	glm::mat4 m_view, m_proj;

	// pure color model and shader program
	shared_ptr<OGLF::Model> m_pc_model;
	GLuint m_purecolor_model_program;

	// render shadows from light's viewing angle
	GLuint m_render_shadow_tex_program;

	// render objects reflected in mirror
	GLuint m_render_mirror_tex_program;

	// mirror model and shader program
	shared_ptr<OGLF::Model> m_plane_model;
	GLuint m_mirror_model_program;

	// shadow framebuffer
	GLuint m_shadow_fb;
	// shadow texture- depth texture since it's monochrome
	GLuint m_shadow_tex;
	// shadow texture2
	GLuint m_shadow_tex2;
	// shadow map view matrix
	glm::mat4 shadowMVP;
	// shadow map2 view matrix
	glm::mat4 shadowMVP2;

	// mirror framebuffer
	GLuint m_mirror_fb;
	// mirror renderbuffer object
	GLuint m_mirror_rbo;
	// depth texture
	GLuint m_mirror_tex;

	// lights
	Light light;
	// light 2
	Light light2;
	// color data buffer
	GLuint colorbuffer;


protected:
	// steal these functions borrowed from the parent classes
	virtual void OnInit() override;
	virtual void OnUpdate() override;

	// all the objects rendered with desired settings in here
	void drawAllTheStuff(bool inMirror=false);

	// render an object using a basic lighting system
	void renderPureObject(
		shared_ptr<OGLF::Model>& pc_model, // model,
		glm::mat4& model_mat, // model transform matrix,
		glm::vec3& obj_Color // obj's color
	);

	// render an object using phong lighting
	void renderPhongObject(
		shared_ptr<OGLF::Model>& phong_model, // model,
		glm::mat4& model_mat, // model transform matrix,
		glm::vec3 && uK,
		glm::vec3 && uSpecularColor,
		glm::float32 uShininess, // phong lighting
		bool inMirror=false, // if in mirror, render to texture
		bool isColorful=false, // activate colorful vertices
		glm::vec3 && obj_Color=glm::vec3(1.,1.,1.) // model color if no colorful vertices
	);


	// initialize texture map for shadow map
	void initShadowMapTex(GLuint& tex, uint width, uint height);
	// input: texture ID, models, transform matrices, light view MVP matrix
	void generateShadowMap(
		GLuint& tex,
		const std::vector<shared_ptr<OGLF::Model>>& models,
		const std::vector<glm::mat4>& model_trans,
		const glm::mat4& shadow_vp,
		uint width, uint height
	);

	// initial setup for mirror texture map
	void initMirrorTex(GLuint& tex, uint width, uint height);
	// render mirror
	void renderMirror(
		GLuint& tex,
		const shared_ptr<OGLF::Model>& mirror_model,
		//const std::vector<glm::mat4>& model_trans,
		uint width, uint height
	);
	//key event
	void keyPressEvent(QKeyEvent *event);

};

#endif
