#version 330 core

uniform mat4 model_view_proj;
uniform mat4 model_view;
uniform mat4 normal_trans;
uniform vec3 lightPosition;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;


out VS_OUT{
	
	vec3 vN; // normal vector
	vec3 vL; // vector from point to light
    vec3 vE; // vector from point to eye
}vs;



void main() {
	
	gl_Position = model_view_proj * vec4(position,1);
	vs.vN = vec3(normalize(normal_trans * vec4(position, 0)));
	
	vec4 ECposition = model_view * vec4( position, 1.0);
	vs.vL = lightPosition - ECposition.xyz;		// vector from the point to the light position
	vs.vE = vec3(0.0) - ECposition.xyz;	// vector from the point to the eye position 	
}
