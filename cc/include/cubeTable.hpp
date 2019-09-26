#pragma once

#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <vector>
#include <bitset>

// how many dimensions? either 3 or 4
#define DIMENSION 3
#define POW2(DIM) (1<<DIM)
// grab a single bit value and return 1 or 0
#define getBit(n,bit) ((n>>bit)&1)
// extract a pair of adjacent bits from a larger number
#define getBit2(n,bit) ((n>>bit)&3)


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
#if DIMENSION == 4
		genTable4D();
#else
		genTable3D();
#endif
	};

	// destructor
	~CubeTable(){
		deleteTable();
	}

	// return the number of elements in each micro-mesh
	char getSize(int idx);
	// return the location of the array of elements
	char* getArray(int idx);
	
private:
	// to generate our final lookup table, we step the dimension up one at a time
	void genTable2D();
	char* table2D[16];

	void genTable3D();
	char* table3D[256];
	// find the index for a given vertex when attached to another face
	char congruentEdge3D(char idx);
	// given a ring of mesh verts on a cube, interpolate them into a triangle mesh
	void interpTris(char vertsByCube[], int ringStart, int ringSize);

	void genTable4D();
	char* table4D[1<<(16-3)];
	
	// lots of space to free from the heap when we're done!
	void deleteTable();
};

