#ifndef LOADINGSHADERS_H_
#define LOADINGSHADERS_H_



#include <vector>
#include <string>
#include <tuple>  
#include <memory>
#include <array>
#include <GL/glew.h>

#include <map>

using namespace std;

namespace OGLF
{
	// load shaders & attach to a new program, note you have to delete program manually
	GLuint CreateProgram(vector<tuple<string, GLenum>> shadernames);
	GLuint CreateProgram(string vertexfilename, string fragmentfilename);

}

#endif