#ifndef CUBEMAP_H_
#define CUBEMAP_H_

#include <string>
#include <iostream>

#include "CImg/CImg.h"
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "ShaderFunc.h"
#include "toolFunction.h"


using std::string;
using glm::vec3;
using glm::vec2;
using glm::mat4;


class Cubemap
{
public:
	Cubemap(){}
	void setup();
	void render(mat4 &in);
	void init(string path);


	GLuint _textureID;


private:
	string _path;

	int _vs,_fs;
	int _program;
	int _cubeLength;
	int _width,_height;

	GLuint _vbo_vertexes;
	GLuint _ibo_indices;

	GLint _uniform_PVM,_attrib_vertex,_uniform_cubemap;
	GLuint _texture2D;

};



#endif
