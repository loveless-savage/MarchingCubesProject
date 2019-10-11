#pragma once

#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <vector>
#include <bitset>

// how many dimensions? 3 of course
#define DIMENSION 3
#define POW2(DIM) (1<<DIM)
// grab a single bit value and return 1 or 0
#define getBit(n,bit) ((n>>bit)&1)
// extract a pair of adjacent bits from a larger number
#define getBit2(n,bit) ((n>>bit)&3)


// print the value of an edge in readable format, given face-based ID
void hrEdgeIdx(char idx);

// find a value in an array, starting at a given position
int findByIdx(char array[], int arraySize, char idx, int startPos);


class CubeTable {
public:
	// initialize CubeTable
	CubeTable(
		// receive parameters
	)//:
		// initialize member variables
	{
		// run on initialization
		genTable3D();
	};

	// destructor
	~CubeTable(){
		deleteTable();
	}

	// return the number of elements in each micro-mesh
	char getNumVerts(int idx);
	char getNumIndices(int idx);
	// return the location of the array of elements
	char* getVertexArray(int idx);
	char* getIndexArray(int idx);

	// diagnostic printout
	void diag(int idx);
	
private:
	// to generate our final lookup table, we step the dimension up one at a time
	void genTable2D();
	char* table2D[16];

	void genTable3D();
	char* table3D[256*2]; // alternate between arrays of verts and indices
	// find the index for a given vertex when attached to another face
	char congruentEdge3D(char idx);
	// convert mVertexIdx from face-based to universal cube coords
	char faceCoordTo3DCoord(char idx);
	// given a ring of mesh verts on a cube, interpolate them into a triangle mesh
	void interpTris(int ringStart, int ringSize, char indicesByCube[], int& numIndices);
	
	// lots of space to free from the heap when we're done!
	void deleteTable();
};

