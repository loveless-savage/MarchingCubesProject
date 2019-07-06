#version 330 core

uniform mat4 model_view_proj;
uniform mat4 model;
uniform mat4 normal_trans;


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;




void main() {
	
	gl_Position = model_view_proj * vec4(position,1);
}