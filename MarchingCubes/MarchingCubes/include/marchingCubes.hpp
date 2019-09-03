#pragma once

#include <loadingModel.hpp>


class MarchingCubes {
public:
	// initialize MarchingCubes
	MarchingCubes(
		// receive parameters
		float frameSize,
		int cellResolution
	):
		// initialize member variables
		frameSize(frameSize),
		cellResolution(cellResolution)
	{
		// run on initialization
		
	};

	// generate lookup table for per-cell primitives
	void genLookupTable();

	// generate mesh with normals from given vector field
	void genModel();
	
private:
	// frame size parameters
	float frameSize;
	int cellResolution;
	
	// lookup table
	char lookuptable[256];
};

#endif
