#version 330

uniform vec3  uColor;			// object color

void main() {
	
	gl_FragColor = vec4(uColor,  1. );
}