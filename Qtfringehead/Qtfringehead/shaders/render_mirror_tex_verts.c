#version 330 core

uniform mat4 model_view_proj;
uniform mat4 model;
uniform mat4 normal_trans;

uniform vec3 lightPosition;
uniform vec3 lightPosition2;
uniform int light_type;
uniform int light_type2;
//shadow map light view matrices
uniform mat4 shadowMVP;
uniform mat4 shadowMVP2;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 3) in vec3 vertexColor;


out VS_OUT{
	vec3 surfaceColor; // color extracted from interpolation
	vec3 vN;  //normal vector
	vec3 vL; // vector from point to light
	vec3 vL2;
    vec3 vE; // vector from point to eye
	vec4 shadowCoord; //shadow map coordinate
	vec4 shadowCoord2;
}vs;


void main() {
	// turn uniform value into a forwarded variable
	vs.surfaceColor = vertexColor;
	
	gl_Position = model_view_proj * vec4(position,1);
	vs.vN = vec3(normalize(normal_trans * vec4(normal, 0)));
	
	vec4 ECposition = model * vec4( position, 1.0);
	
	//SPOT 0,POINT 1,DIRECTION 2
	if(2 == light_type){
		vs.vL = lightPosition;
	}else if(1 == light_type){
		vs.vL = lightPosition - ECposition.xyz;		// vector from the point to the light position
	}else{
		vs.vL = lightPosition - ECposition.xyz;	
	}
	
	if(2 == light_type2){
		vs.vL2 = lightPosition2;
	}else if(1 == light_type2){
		vs.vL2 = lightPosition2 - ECposition.xyz;		// vector from the point to the light position
	}else{
		
	    vs.vL2 = lightPosition2 - ECposition.xyz;	
	}
	
	vs.vE = vec3(0.0) - ECposition.xyz;	// vector from the point to the eye position 	
	
	vs.shadowCoord = (shadowMVP * model * vec4(position,1));
	vs.shadowCoord2 = (shadowMVP2 * model * vec4(position,1));
}
