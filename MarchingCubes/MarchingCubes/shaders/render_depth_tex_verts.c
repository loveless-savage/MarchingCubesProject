#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;

uniform mat4 depthMVP;
uniform mat4 model;

void main(){
	
   gl_Position =  depthMVP * model * vec4(position,1);
   //gl_Position = vec4(0.1);
}