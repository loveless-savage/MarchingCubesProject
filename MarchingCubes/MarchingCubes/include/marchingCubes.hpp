#pragma once

// use a lookup table?
#define LOOKUP_TABLE 1
// use reflections to reduce the number of cases?
#define REFLECTION_COMPRESSION 0

#include <cstdlib>
//#include <cstdint>
#include <iostream>
#include <cmath>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include "cellNum.hpp"
#include "cubeTable.hpp"
#include "loadingModel.hpp"
#include "implicitFormula.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using OGLF::Vertex;



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
		CubeTable* inputTable
	):
		// initialize member variables
		frameSize(frameSize),
		lookupTable(inputTable)
	{
		// run on initialization
	};

	// generate mesh with normals from given vector field
	void genModel();

	// diagnostic printout of the mesh
	void diag();

	// return a scaling matrix to normalize frame size
	glm::mat4 frameScaleMatrix();

	// render the generated model from the GL wrapper
	void Draw(GLuint program);
	
private:
	// array of grid points
	vec4 pts[CELLNUM+1][CELLNUM+1][CELLNUM+1];
	// ...and the associated normals
	vec3 norms[CELLNUM+1][CELLNUM+1][CELLNUM+1];

	// temporary array to hold each voxel's array of mesh vertices
	char* vertsTmp[CELLNUM*CELLNUM*CELLNUM];
	// temporary array of each voxel's array of mesh indices
	char* indicesTmp[CELLNUM*CELLNUM*CELLNUM];
	// also keep track of each voxel's array sizes
	char vertSizes[CELLNUM*CELLNUM*CELLNUM];
	char indexSizes[CELLNUM*CELLNUM*CELLNUM];
	// we need to keep a full table of indices as well, so we can eliminate duplicates
	GLuint offsettedIndices[CELLNUM*CELLNUM*CELLNUM][12];
	char vertSums[CELLNUM*CELLNUM*CELLNUM];

	// read from linear arrays with 3D coordinates
	int coord(int k, int j, int i);

	// read from vertsTmp[] using indices out of indicesTmp[]
	char vertsTmp_byIdx(int k, int j, int i, int idxNum);


	// final array of generated mesh vertices; this will be given to opengl
	std::vector<Vertex> verts;
	// array of generated mesh indices
	std::vector<GLuint> indices;


	// push a new index onto the indices vector, unless it is a duplicate
	void pushIdxWithoutDups(int k, int j, int i, int idxNum);

	// push a new vertex onto the verts vector, but first convert it to global space coordinates!
	void pushGlobalCoordinatesVertex(int k, int j, int i, int idx);


	
	// the viewing frame is a cube whose coordinates are (frameSize,frameSize,frameSize) and (-frameSize,-frameSize,-frameSize)
	float frameSize; // width of the cube is 2*frameSize
	int cellNum=CELLNUM;

	// the location of our lookup table
	CubeTable* lookupTable;
	// read from lookup table
	void readLookupTable(int idx, int coord);


	// set up OpenGL buffers
	void setupGL();
	GLuint vbo_, vao_, ebo_;

	

	// real-time interpolation: deprecated
#if LOOKUP_TABLE == 0
	// interpolate mesh vertices, lines, triangles, and simplices
	void interpVerts(int k, int j, int i, int axis);
	void interpEdges(int k, int j, int i, int axis1, int axis2);
	void interpTris(int k, int j, int i, int axis_norm);
	void interpSimp(int k, int j, int i);
#endif

};

