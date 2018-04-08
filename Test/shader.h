#pragma once


#include <string>
#include <vector>
class Shader {
public:
	static GLuint createShader(GLenum eShaderType, const std::string &strShaderFile);
	static GLuint createShader(GLenum eShaderType, const char* fileName);
	static GLuint createProgram(const std::vector<GLuint> &shaderList);
};

