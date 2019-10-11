#pragma once
#include <cmath>
#include <glm/glm.hpp>
#include "cellNum.hpp"

using glm::vec3;
using glm::vec4;

// this is the implicit formula that defines the marching cubes test
// it is currently set to analyze a scalar function
// to change the scalar function, go to src/cellNum.cpp
float scalarEquation( vec3 input );

// this function executes the formula over all voxels in the frame space
// it is called during MarchingCubes::genModel() from marchingCubes.cpp
void implicitFormula(int cellNum, float frameSize, vec4 pts[CELLNUM+1][CELLNUM+1][CELLNUM+1], vec3 norms[CELLNUM+1][CELLNUM+1][CELLNUM+1]);
