#include "implicitFormula.hpp"

// change the equation here! the test compares this scalar value to zero
float scalarEquation( vec3 input ){
	return input.x*input.x + input.y*input.y + input.z*input.z - 16.f; // sphere
	//return sin(input.x)+sin(input.y)+sin(input.z); // sponge
}

void implicitFormula(int cellNum, float frameSize, vec4 pts[CELLNUM+1][CELLNUM+1][CELLNUM+1], vec3 norms[CELLNUM+1][CELLNUM+1][CELLNUM+1]){
	vec3 globalCoords;
	float stepLength = 2*frameSize/cellNum;
	// first do the literal scalar calculations
	for(int k=0; k<cellNum+1; k++){
	for(int j=0; j<cellNum+1; j++){
	for(int i=0; i<cellNum+1; i++){
		// deduce actual world space coordinates from grid index
		globalCoords = vec3( -frameSize );
		globalCoords.x += stepLength*i;
		globalCoords.y += stepLength*j;
		globalCoords.z += stepLength*k;

		// vector equation
		pts[k][j][i].x = sin(globalCoords.x);
		pts[k][j][i].y = sin(globalCoords.y);
		pts[k][j][i].z = sin(globalCoords.z);
		// scalar equation
		pts[k][j][i].w = scalarEquation(globalCoords);

	}}}

	// now differentiate and find normals to the vector field
	// currently it is set up to analyze only the scalar component of pts[][][]
	for(int k=0; k<cellNum+1; k++){
	for(int j=0; j<cellNum+1; j++){
	for(int i=0; i<cellNum+1; i++){
		// deduce actual world space coordinates from grid index
		globalCoords = vec3( -frameSize );
		globalCoords.x += stepLength*i;
		globalCoords.y += stepLength*j;
		globalCoords.z += stepLength*k;


		// find difference between scalar values on either side of the focus point, in each dimension in turn
		// x
		if(i==0){ // edge behavior
			norms[k][j][i].x = pts[k][j][i+1].w-pts[k][j][i].w;
			norms[k][j][i].x *= 2;
		}else if(i==cellNum){ // edge behavior
			norms[k][j][i].x = pts[k][j][i].w-pts[k][j][i-1].w;
			norms[k][j][i].x *= 2;
		}else{
			norms[k][j][i].x = pts[k][j][i+1].w-pts[k][j][i-1].w;
		}
		// y
		if(j==0){ // edge behavior
			norms[k][j][i].y = pts[k][j+1][i].w-pts[k][j][i].w;
			norms[k][j][i].y *= 2;
		}else if(j==cellNum){ // edge behavior
			norms[k][j][i].y = pts[k][j][i].w-pts[k][j-1][i].w;
			norms[k][j][i].y *= 2;
		}else{
			norms[k][j][i].y = pts[k][j+1][i].w-pts[k][j-1][i].w;
		}
		// z
		if(k==0){ // edge behavior
			norms[k][j][i].z = pts[k+1][j][i].w-pts[k][j][i].w;
			norms[k][j][i].z *= 2;
		}else if(k==cellNum){ // edge behavior
			norms[k][j][i].z = pts[k][j][i].w-pts[k-1][j][i].w;
			norms[k][j][i].z *= 2;
		}else{
			norms[k][j][i].z = pts[k+1][j][i].w-pts[k-1][j][i].w;
		}

		// normalize
		norms[k][j][i] /= stepLength*2;

	}}}
}
