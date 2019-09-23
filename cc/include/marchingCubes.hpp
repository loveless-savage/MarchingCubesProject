#pragma once

// use a lookup table?
#define LOOKUP_TABLE 1

#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <vector>
#include "cubeTable.hpp"


#define CELLNUM 25


typedef struct{
	float x=0;
	float y=0;
	float z=0;
	//float w;
	float f=0;
	std::vector<int> verts;
}vec4;

// reflections to align each index to match one of the four primary cases
int reflectIdx(int idx, int axis);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
class MarchingCubes {
public:
	// initialize MarchingCubes
	MarchingCubes(
		// receive parameters
		float frameSize,
		CubeTable& inputTable
	):
		// initialize member variables
		frameSize(frameSize),
		lookupTable(&inputTable)
		//pts(new vec4[cellNum][cellNum][cellNum])//[cellNum+1])
	{
		// run on initialization
	};

	// generate mesh with normals from given vector field
	void genModel();

	// diagnostic printout of the mesh
	void diag();
	
private:
	// array of points
	vec4 pts[CELLNUM+1][CELLNUM+1][CELLNUM+1];//[CELLNUM+1];

	// array of generated mesh vertices
	float verts[CELLNUM+1][CELLNUM+1][CELLNUM+1][3];

	// the vector field operator
	void do_u(int x,int y,int z);//,int w);

	// frame size parameters
	float frameSize;
	int cellNum=CELLNUM;

	// the location of our lookup table
	CubeTable* lookupTable;
	// read from lookup table
	void readLookupTable(int idx);

	// interpolate mesh vertices, lines, triangles, and simplices
	void interpVerts(int k, int j, int i, int axis);
	void interpEdges(int k, int j, int i, int axis1, int axis2);
	void interpTris(int k, int j, int i, int axis_norm);
	void interpSimp(int k, int j, int i);

};

