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
#include <iostream>

class ShadowMapGL : public OGLF::OGLFramework{

public:
	ShadowMapGL(){}
	~ShadowMapGL(){
		glDeleteProgram(m_model_program);
	}

private:
	
	shared_ptr<OGLF::Model> m_dragon_model, m_cube_model;
	//shader program
	GLuint m_model_program;
	//view matrix and projective matrix
	glm::mat4 m_view, m_proj;

	//pure color model and shader program
	shared_ptr<OGLF::Model> m_pc_model;
	GLuint m_pc_model_program;

	//shaders to render light's view point
	GLuint m_render_depth_tex_program;
	//objects
	shared_ptr<OGLF::Model> m_plane_model;

	//frame buffeer
	GLuint m_framebuffer;
	//depth texture
	GLuint m_depth_tex;
	//depth texture2
	GLuint m_depth_tex2;
	//shadow map;
	glm::mat4 depthMVP;
	//shadow map2
	glm::mat4 depthMVP2;
	//lights
	Light light;
	//light 2
	Light light2;

	
protected:
	//overide function
    virtual void OnInit() override;
	virtual void OnUpdate() override;

	//input: model, model transform matrix, obj's color
	virtual void renderPureObject(
		shared_ptr<OGLF::Model>& pc_model,
		glm::mat4& model_mat,
		glm::vec3& obj_Color);

	//input: model, model transform matrix, obj's material, phong lighting
	virtual void renderPhongObject(
		shared_ptr<OGLF::Model>& phong_model,
		glm::mat4& model_mat,
		glm::vec3& obj_Color,
		glm::vec3& uK,
		glm::vec3& uSpecularColor,
		glm::float32 uShininess
		);

	//initialize texture map for shadow map
	void initShadowMapTex(GLuint& tex, uint width, uint height);
	//input:modes, models' transform matrix ptr, light view projection matr
	//shadowmap width, height, output:tex
	//output: texture
	void generateShadowMap(
		GLuint& tex,
		const std::vector<shared_ptr<OGLF::Model>>& models,
		const std::vector<glm::mat4>& model_trans,
		const glm::mat4& depth_vp,
		uint width, uint height);

	//key event
	void keyPressEvent(QKeyEvent *event);

};

#endif