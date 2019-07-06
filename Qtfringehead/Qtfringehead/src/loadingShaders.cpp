
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "loadingShaders.hpp"

//using std::map;
//using std::shared_ptr;
//using std::array;

GLuint OGLF::CreateProgram(string vertexfilename, string fragmentfilename)
{
	std::vector<std::tuple<std::string, GLenum>> shaders;
	shaders.push_back(std::tuple<std::string, GLenum>(vertexfilename, GL_VERTEX_SHADER));
	shaders.push_back(std::tuple<std::string, GLenum>(fragmentfilename, GL_FRAGMENT_SHADER));
	return CreateProgram(shaders);
}

GLuint OGLF::CreateProgram(vector<tuple<string, GLenum>> shadernames)
{
	// create program
	GLuint program = glCreateProgram();
	vector<GLuint> shaders;
	// setup shader deletion
	auto delete_shaders = [](const vector<GLuint>& shaders)
	{
		for (auto s : shaders) {
			glDeleteShader(s);
		}
	};
	// setup compile error checker
	auto check_compile = [](GLint shader, std::string* info)->bool
	{
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			// get info size
			GLint logsize = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);

			// get info
			GLchar* errinfo = new GLchar[logsize];
			glGetShaderInfoLog(shader, logsize, &logsize, errinfo);
			*info = std::string(errinfo, errinfo + logsize);
			delete[] errinfo;
			return false;
		}
		return true;
	};

	// read each shader
	for (auto shaderinfo : shadernames)
	{
		string sname = get<0>(shaderinfo);
		GLenum type = get<1>(shaderinfo);
		// open file stream
		std::ifstream sfile(sname);
		if (!sfile)
			throw runtime_error("read " + sname + " failed");

		// read content
		std::string content;
		std::string line;
		while (std::getline(sfile, line))
			content += line + '\n';

		// create shader
		GLuint shader = glCreateShader(type);
		shaders.push_back(shader);
		const GLchar* str = content.c_str();
		glShaderSource(shader, 1, &str, NULL);
		glCompileShader(shader);

		// check compile result
		std::string info;
		if (!check_compile(shader, &info))
		{
			delete_shaders(shaders);
			glDeleteProgram(program);
			throw runtime_error("A error ocurred when compile "
				+ sname + ":\n" + info);
		}
		// attach to program
		glAttachShader(program, shader);
	}
	// link
	glLinkProgram(program);
	// clean up
	delete_shaders(shaders);
	return program;
}
