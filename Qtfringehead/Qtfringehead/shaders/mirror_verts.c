#version 330 core

uniform mat4 model_view_proj;

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 texCoord;


out vec3 vColor;


void main() {
	gl_Position = model_view_proj * vec4(position,1);
	
	vColor = 0.5*vec3(gl_Position)+vec3(0.5);
}
