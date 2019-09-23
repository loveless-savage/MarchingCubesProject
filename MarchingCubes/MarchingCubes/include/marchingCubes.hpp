#pragma once

#include <cmath>
#include <cstdlib>
#include <vector>

#define CELLNUM 20


typedef struct{
	float x=0;
	float y=0;
	float z=0;
	//float w;
	float f=0;
	std::vector<int> verts;
}vec4;


class MarchingCubes {
public:
	// initialize MarchingCubes
	MarchingCubes(
		// receive parameters
		float frameSize
	):
		// initialize member variables
		frameSize(frameSize)
		//pts(new vec4[cellNum][cellNum][cellNum])//[cellNum+1])
	{
		// run on initialization
	};

	// generate mesh with normals from given vector field
	void genModel();
	
private:
	// array of points
	vec4 pts[CELLNUM][CELLNUM][CELLNUM];//[CELLNUM];

	// array of generated mesh vertices
	float verts[CELLNUM][CELLNUM][CELLNUM][3];

	// the vector field operator
	void do_u(int x,int y,int z);//,int w);

	// frame size parameters
	float frameSize;
	int cellNum=CELLNUM;

	// interpolate mesh vertices, lines, triangles, and simplices
	void interpVerts(int k, int j, int i, int axis);
	void interpEdges(int k, int j, int i, int axis1, int axis2);
	void interpTris(int k, int j, int i, int axis_norm);
	void interpSimp(int k, int j, int i);

};

