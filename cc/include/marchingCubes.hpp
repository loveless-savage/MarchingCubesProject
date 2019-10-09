#pragma once

// use a lookup table?
#define LOOKUP_TABLE 1
// use reflections to reduce the number of cases?
#define REFLECTION_COMPRESSION 0

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
	float f=0;
}vec3;

#if REFLECTION_COMPRESSION
// reflections to align each index to match one of the four primary cases
int reflectIdx(int idx, int axis);
#endif


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
	// array of grid points
	vec3 pts[CELLNUM+1][CELLNUM+1][CELLNUM+1];

	// temporary array to hold each voxel's array of mesh vertices
	char* vertsTmp[CELLNUM*CELLNUM*CELLNUM];
	// temporary array of each voxel's array of mesh indices
	char* indicesTmp[CELLNUM*CELLNUM*CELLNUM];
	// also keep track of each voxel's array sizes
	char vertSizes[CELLNUM*CELLNUM*CELLNUM];
	char indexSizes[CELLNUM*CELLNUM*CELLNUM];
	// keep a running count of the total number of mesh vertices having been added before each voxel
	char vertSums[CELLNUM*CELLNUM*CELLNUM];

	// read from linear arrays with 3D coordinates
	int coord(int k, int j, int i);

	// read from vertsTmp[] using indices out of indicesTmp[]
	char vertsTmp_byIdx(int k, int j, int i, int idxNum);


	// final array of generated mesh vertices; this will be given to opengl
	std::vector<float> verts;
	// array of generated mesh indices
	std::vector<char> indices;

	// push a new index onto the indices vector, unless it is a duplicate
	void pushIdxWithoutDups(int k, int j, int i, int idxNum);

	// push a new vertex onto the verts vector, but first convert it to global space coordinates!
	void pushGlobalCoordinatesVertex(int k, int j, int i, int idx);


	// the vector field operator
	void do_u(int k,int j,int i);

	// frame size parameters
	float frameSize;
	int cellNum=CELLNUM;

	// the location of our lookup table
	CubeTable* lookupTable;
	// read from lookup table
	void readLookupTable(int idx, int coord);

	// real-time interpolation
#if LOOKUP_TABLE == 0
	// interpolate mesh vertices, lines, triangles, and simplices
	void interpVerts(int k, int j, int i, int axis);
	void interpEdges(int k, int j, int i, int axis1, int axis2);
	void interpTris(int k, int j, int i, int axis_norm);
	void interpSimp(int k, int j, int i);
#endif

};

