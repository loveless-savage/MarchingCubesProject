the Marching Cubes Algorithm has the following file include structure:

main.cpp			---main function call
│
QtWrapper.hpp/cpp		---setup for Qt window contents and opening/closing; invoke Marching Cubes
│ │
│ ui_QtWrapper.h		---auto-generated; window widget layout
│
ShadowMapGL.hpp/cpp		---perform model loading, shadow and regular rendering
│ │ │ │ │
│ │ │ │ type.hpp		---structs for light classification
│ │ │ │
│ │ │ OGLFramework.hpp/cpp	---setup for mouse interaction and global transformation matrices
│ │ │
│ │ loadingShaders.hpp/cpp	---load files holding GLSL shaders
│ │
│ marchingCubes.hpp/cpp		---execute Marching Cubes in selected space, and setup associated OpenGL buffers
│ │ │ │ │
│ │ │ │ cubeTable.hpp/cpp	---generate lookup table for Marching Cubes at beginning of execution
│ │ │ │
│ │ │ implicitFormula.hpp/cpp	---dictate what test to perform; this file is the place to implement volume data
│ │ ├─┘
│ │ cellNum.hpp			---compiler definition of number of voxels and size of selected space
├─┘
loadingModel.hpp/cpp		---struct Vertex with position, normals, & texture coords; load model files with assimp

Right now implicitFormula.cpp is configured to render a sphere, but it can easily be changed

Currently, the issue with this program is hiding somewhere in marchingCubes.cpp
most likely in the function MarchingCubes::pushIdxWithoutDups(k,j,i,idxNum)

the issue either concerns badly indexing the mesh vertices, calculating the vertices incorrectly, or bad reference to the lookup table.
