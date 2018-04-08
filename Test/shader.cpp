#include "stdafx.h"
#include "shader.h"

#include <fstream>  
#include <sstream>  

//从字符串流构造着色器对象  
GLuint Shader::createShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);//根据类型创建shader，顶点或者片段着色器  
	const char * strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);//绑定shader字符串, count是shader代码段个数，可以连接多个代码段  
	glCompileShader(shader);//编译shader,得到静态的汇编指令，绑定到shader标示符地址  
							//检查shader状态  
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char * strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}
		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}
	return shader;
}
//从文件构造着色器对象  
GLuint Shader::createShader(GLenum eShaderType, const char* fileName)
{
	std::ifstream infile(fileName);
	if (!infile)
	{
		fprintf(stderr, "Could not open file : %s for reading.", fileName);
		return 0;
	}
	std::stringstream  buffer;
	buffer << infile.rdbuf();
	infile.close();
	return Shader::createShader(eShaderType, buffer.str());
}
//构造着色器程序对象  
GLuint Shader::createProgram(const std::vector<GLuint> &shaderList)
{
	GLuint programId = glCreateProgram();//创建program  
	for (std::vector<GLuint>::size_type iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(programId, shaderList[iLoop]);//绑定shader,一个着色器程序可以绑定多个着色器程序  

	glLinkProgram(programId);//链接shader，得到可运行的机器码  
							 //检查program状态，如果有错误打印出错信息  
	GLint status;
	glGetProgramiv(programId, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(programId, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	/*for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
	glDetachShader(programId, shaderList[iLoop]);*/
	return programId;
}