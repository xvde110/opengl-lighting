// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "shader.h"
#include "Mesh.h"
#include <Windows.h>
#include <math.h>
#include <time.h>
//const GLchar* vertexShaderSource = "#version 330 core\n"
//"layout (location = 0) in vec3 position;\n"
//"void main()\n"
//"{\n"
//"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
//"}\0";
//
//const GLchar* fragmentShaderSource = "#version 330 core\n"
//"out vec4 color;\n"
//"void main()\n"
//"{\n"
//"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//"}\n\0";


GLuint VAO, VAO_RENDER;
GLuint VBO,VBO_RENDER;
GLuint EBO;
GLuint TBO, TBO2, grassIMG;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram, shaderProgram2;
GLuint FBO, window[4];
string getFileStr(const char * filename) {
	ifstream infile(filename);
	stringstream buffer;
	buffer << infile.rdbuf();
	infile.close();
	return buffer.str();
}

void coutData(float* mat)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			std::cout << mat[i * 4 + j] << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
//model
void transformMatrix(GLfloat x,GLfloat y,GLfloat z,GLfloat *matrix)
{
	for (int i = 0; i < 16; i++)
		matrix[i] = 0;
	matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0;
	matrix[3] = x; matrix[7] = y; matrix[11] = z;
}
//view
void LookAtMatrix(Vector3 eye, Vector3 look, Vector3 up, float * matrix)
{
	for (int i = 0; i < 16; i++)
		matrix[i] = 0;
	Vector3 z(eye.x - look.x, eye.y - look.y, eye.z - look.z);
	z.normalize();
	Vector3 x(z.cross(up));
	x.normalize();
	Vector3 y(x.cross(z));
	y.normalize();

	matrix[0] = -x.x;
	matrix[4] = -x.y;
	matrix[8] = -x.z;
	matrix[12] = x.dot(eye);

	matrix[1] = y.x;
	matrix[5] = y.y;
	matrix[9] = y.z;
	matrix[13] = -y.dot(eye);

	matrix[2] = z.x;
	matrix[6] = z.y;
	matrix[10] = z.z;
	matrix[14] = -z.dot(eye);

	matrix[3] = matrix[7] = matrix[11] = 0.0f;
	matrix[15] = 1.0f;
}
//projection
void frustumMatrix(float l, float r, float b, float t, float zNear, float zFar, float * matrix)
{
	for (int i = 0; i < 16; i++)
		matrix[i] = 0;
	matrix[0] = 2 * zNear / (r - l);
	matrix[8] = (r + l) / (r - l);
	matrix[5] = 2 * zNear / (t - b);
	matrix[9] = (t + b) / (t - b);
	matrix[10] = -(zFar + zNear) / (zFar - zNear);
	matrix[14] = -(2 * zFar * zNear) / (zFar - zNear);
	matrix[11] = -1;
	matrix[15] = 0;
}
GLfloat cameraPos[] = { 2,2,4 };
void setParameter(GLint index) {


	GLfloat mat[16];
	GLint x = rand() % 7 - 3;
	GLint y = rand() % 7 - 3;
	GLint z = rand() % 7 - 3;
	transformMatrix(x, y, z, mat);
	//transformMatrix(0, 0, 0, mat);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_TRUE, mat);
	LookAtMatrix(Vector3(cameraPos[0], cameraPos[1], cameraPos[2]), Vector3(0, 0, 0), Vector3(0, 1, 0), mat);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, mat);
	coutData(mat);
	frustumMatrix(-1, 1, -1, 1, 1, 200, mat);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, GL_FALSE, mat);
	coutData(mat);

	glUniform3f(glGetUniformLocation(shaderProgram, "cameraPos"), cameraPos[0], cameraPos[1], cameraPos[2]);



	GLint matDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
	GLint matSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
	GLint matShineLoc = glGetUniformLocation(shaderProgram, "material.shininess");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TBO);
	glUniform1i(matDiffuseLoc,0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TBO2);
	glUniform1i(matSpecularLoc, 1);
	glUniform1f(matShineLoc, 32.0f);

	GLfloat lightPos[] = { 0,0,5 };
	GLfloat lightDir[] = { 0.0,0.0,-1.0 };
	GLint lightPosition = glGetUniformLocation(shaderProgram, "light.position");
	GLint lightDirection = glGetUniformLocation(shaderProgram, "light.direction");
	GLint lightAmbientLoc = glGetUniformLocation(shaderProgram, "light.ambient");
	GLint lightDiffuseLoc = glGetUniformLocation(shaderProgram, "light.diffuse");
	GLint lightSpecularLoc = glGetUniformLocation(shaderProgram, "light.specular");
	glUniform3f(lightPosition, lightPos[0], lightPos[1], lightPos[2]);
	glUniform3f(lightDirection, lightDir[0], lightDir[1], lightDir[2]);
	glUniform3f(lightAmbientLoc, 0.7f, 0.7f, 0.7f);
	glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f); // Let's darken the light a bit to fit the scene 
	glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);


	glUniform1f(glGetUniformLocation(shaderProgram, "light.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.linear"), 0.07);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.quadratic"), 0.017);
	glUniform1f(glGetUniformLocation(shaderProgram, "light.cutoff"), cos(50.0f/360*3.1415926));
	glUniform1f(glGetUniformLocation(shaderProgram, "light.outercutoff"), cos(60.0f / 360 * 3.1415926));
}
GLubyte *loadBMPImage(char * filename, GLint * iWidth, GLint * iHeight, GLint * icomponents, GLenum * eFormat)
{
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL)
		return NULL;
	if (fseek(fp, sizeof(BITMAPFILEHEADER), 0))
		return NULL;
	BITMAPINFOHEADER infoHead;
	fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);
	*iWidth = infoHead.biWidth;
	*iHeight = infoHead.biHeight;
	GLint linebyte = (*iWidth * 24 / 8 + 3) / 4 * 4;
	GLubyte *pBits = new unsigned char[linebyte*(*iHeight)];
	fread(pBits, sizeof(char), linebyte*(*iHeight), fp);
	fclose(fp);

	*icomponents = GL_RGB8;
	*eFormat = GL_BGR_EXT;

	return pBits;
}

GLfloat vertices[24*8];
void userInit()
{
	//getPosition
	{
		static const GLfloat vertex_list[][3] = {
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f,   1.0f, -1.0f,
		1.0f,   1.0f, -1.0f,
		-1.0f, -1.0f,   1.0f,
		1.0f, -1.0f,   1.0f,
		-1.0f,   1.0f,   1.0f,
		1.0f,   1.0f,   1.0f,
		};
		// 将要使用的顶点的序号保存到一个数组里面
		static const GLint index_list[][4] = {
			0, 2, 3, 1,
			0, 4, 6, 2,
			0, 1, 5, 4,
			4, 5, 7, 6,
			1, 3, 7, 5,
			2, 6, 7, 3,
		};
		static const GLfloat normal_list[][3] = {
			0,0,-1,
			-1,0,0,
			0,-1,0,
			0,0,1,
			1,0,0,
			0,1,0
		};
		for (int i = 0; i < 6; ++i)          // 有六个面，循环六次
		{
			for (int j = 0; j < 4; ++j)      // 每个面有四个顶点，循环四次
			{
				//position
				vertices[i * 32 + j * 8 + 0] = vertex_list[index_list[i][j]][0];
				vertices[i * 32 + j * 8 + 1] = vertex_list[index_list[i][j]][1];
				vertices[i * 32 + j * 8 + 2] = vertex_list[index_list[i][j]][2];
				//normal
				vertices[i * 32 + j * 8 + 3] = normal_list[i][0];
				vertices[i * 32 + j * 8 + 4] = normal_list[i][1];
				vertices[i * 32 + j * 8 + 5] = normal_list[i][2];
				//texcoords
				switch (j % 4)
				{
				case 0:vertices[i * 32 + j * 8 + 6] = 1.0; vertices[i * 32 + j * 8 + 7] = 1.0; break;
				case 1:vertices[i * 32 + j * 8 + 6] = 1.0; vertices[i * 32 + j * 8 + 7] = 0.0; break;
				case 2:vertices[i * 32 + j * 8 + 6] = 0.0; vertices[i * 32 + j * 8 + 7] = 0.0; break;
				case 3:vertices[i * 32 + j * 8 + 6] = 0.0; vertices[i * 32 + j * 8 + 7] = 1.0; break;
				}
			}
		}
	}

	

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);
	

	GLubyte *pImage = NULL;
	GLint  iWidth;
	GLint  iHeight;
	GLint  icomponents;
	GLenum  eFormat;

	//texture init
	glGenTextures(1, &TBO);
	glBindTexture(GL_TEXTURE_2D, TBO);
	

	pImage = loadBMPImage("box_diffuse.bmp", &iWidth, &iHeight, &icomponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, icomponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pImage);
	glGenerateMipmap(GL_TEXTURE_2D);

	glGenTextures(1, &TBO2);
	glBindTexture(GL_TEXTURE_2D, TBO2);

	pImage = loadBMPImage("box_specular.bmp", &iWidth, &iHeight, &icomponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, icomponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pImage);
	glGenerateMipmap(GL_TEXTURE_2D);

	free(pImage);
	glBindTexture(GL_TEXTURE_2D, 0);
	//texture setting
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//init shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	string str = getFileStr("vertexShader.glsl");
	const char *vertexShaderStr = str.c_str();

	glShaderSource(vertexShader, 1, &vertexShaderStr, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512] = {' '};
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "error in vertexshader:" << infoLog << std::endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	str = getFileStr("fragmentShader.glsl");
	const char *fragmentShaderStr = str.c_str();


	glShaderSource(fragmentShader, 1, &fragmentShaderStr, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "error in fragmentshader:" << infoLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "error in linkprogram:" << infoLog << endl;
	}

	glBindVertexArray(0);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	str = getFileStr("renderVertex.glsl");
	vertexShaderStr = str.c_str();

	glShaderSource(vertexShader, 1, &vertexShaderStr, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "error in vertexshader:" << infoLog << std::endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	str = getFileStr("renderFragment.glsl");
	fragmentShaderStr = str.c_str();


	glShaderSource(fragmentShader, 1, &fragmentShaderStr, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "error in fragmentshader:" << infoLog << std::endl;
	}

	shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShader);
	glLinkProgram(shaderProgram2);

	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		std::cout << "error in linkprogram:" << infoLog << endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//////////
	glGenTextures(4, window);
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, window[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	//fbo
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	for(int i=0;i<4;i++)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, window[i], 0);
	GLuint attachment[] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachment);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//
	GLfloat rect[8] = { 1,-1,1,1,-1,1,-1,-1 };
	glGenVertexArrays(1, &VAO_RENDER);
	glBindVertexArray(VAO_RENDER);
	glGenBuffers(1, &VBO_RENDER);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_RENDER);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*2, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
//调整窗口大小回调函数  
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	/*glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(2, 3, 4, 0, 0, 0, 0, 1, 0);

	float mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	coutData(mat);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 1, 200);
	glGetFloatv(GL_PROJECTION_MATRIX, mat);
	coutData(mat);*/
}

//绘制回调函数  
void display(void)
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDepthMask(GL_FALSE);




	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	for (int i = 0; i < 4; i++)
	{
		setParameter(0);
		glDrawArrays(GL_QUADS, 0, 4 * 6);
	}
	glBindVertexArray(0);

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(shaderProgram2);
	for (int i = 0; i < 4; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, window[i]);
		char t[5] = "win";
		t[3] = i + '0'; t[4] = '\0';
		glUniform1i(glGetUniformLocation(shaderProgram2, t), i);
	}
	glBindVertexArray(VAO_RENDER);
	glDrawArrays(GL_QUADS, 0, 4);
	glBindVertexArray(0);

	glutSwapBuffers();

	//Sleep(1000);

	//glutPostRedisplay();//不断刷新  
}

void mouseFunc(int button,int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		glutPostRedisplay();
}

int main(int argc, char **argv)
{
	srand(time(0));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Triangle demo");

	glewInit();
	userInit();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouseFunc);

	glutMainLoop();
	return 0;
}
