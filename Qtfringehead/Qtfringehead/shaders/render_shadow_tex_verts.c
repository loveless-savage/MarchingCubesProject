#version 330 core

// Input vertex data, unique to each vertex
layout(location = 0) in vec3 position;

// uniform variables, each vertex gets same values in a single render
uniform mat4 shadowMVP;
uniform mat4 model;

void main(){
	
   gl_Position =  shadowMVP * model * vec4(position,1);
   //gl_Position = vec4(0.1);
}
