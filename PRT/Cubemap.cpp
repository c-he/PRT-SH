#include "Cubemap.h"

void Cubemap::init(string path)
{
	std::string names[6] = {"/right.bmp", "/left.bmp", "/top.bmp", "/bottom.bmp", "/front.bmp", "/back.bmp"};

	int width, height;

	GLubyte *pixels[6];

	for(int i = 0; i < 6; i++)
	{
		pixels[i] = loadBMP(path + names[i],width,height);
	}

	glGenTextures(1, &_textureID);

	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

	for(int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_RGB,width,height,0,GL_BGR,GL_UNSIGNED_BYTE,pixels[i]);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	//return true;
}


void Cubemap::setup()
{

	GLint link_ok = GL_FALSE;

	if((_vs = create_shader("cubemap_v.glsl",GL_VERTEX_SHADER)) == 0) 
		std::cout <<"Shader Error!";
	if((_fs = create_shader("cubemap_f.glsl",GL_FRAGMENT_SHADER)) == 0) 
		std::cout <<"Shader Error!";

	_program = glCreateProgram();
	glAttachShader(_program,_vs);
	glAttachShader(_program,_fs);
	glLinkProgram(_program);

	glGetProgramiv(_program,GL_LINK_STATUS,&link_ok);
	if(!link_ok)
	{
		std::cout <<"Shader Program Link Error!";
	}
	// cube vertices for vertex buffer object

	std::string query_name = "PVM";
	_uniform_PVM = glGetUniformLocation(_program,query_name.c_str());
	if (_uniform_PVM == -1) 
	{
		fprintf(stderr, "Could not bind uniform %s\n", query_name);
	}

	query_name = "vertex";
	_attrib_vertex = glGetAttribLocation(_program,query_name.c_str());
	if(_attrib_vertex == -1)
	{
		fprintf(stderr, "Could not bind attribute %s\n", query_name);
	}

	query_name = "cubemap";
	_uniform_cubemap = glGetUniformLocation(_program,query_name.c_str());
	if(_uniform_cubemap == -1)
	{
		fprintf(stderr,"Could not bind uniform %s\n",query_name.c_str());
	}

	GLfloat cube_vertices[] = 
	{
		-4000.0f,  4000.0f, -4000.0f,  4000.0f,  4000.0f, -4000.0f,  4000.0f,  4000.0f,  4000.0f, -4000.0f,  4000.0f,  4000.0f, // +Y
		-4000.0f, -4000.0f,  4000.0f, -4000.0f, -4000.0f, -4000.0f, -4000.0f,  4000.0f, -4000.0f, -4000.0f,  4000.0f,  4000.0f, // -X
		-4000.0f, -4000.0f, -4000.0f,  4000.0f, -4000.0f, -4000.0f,  4000.0f,  4000.0f, -4000.0f, -4000.0f,  4000.0f, -4000.0f,  // -Z
		 4000.0f, -4000.0f, -4000.0f,  4000.0f, -4000.0f,  4000.0f,  4000.0f,  4000.0f,  4000.0f,  4000.0f,  4000.0f, -4000.0f, // +X
		-4000.0f, -4000.0f,  4000.0f,  4000.0f, -4000.0f,  4000.0f,  4000.0f, -4000.0f, -4000.0f, -4000.0f, -4000.0f, -4000.0f, // -Y
		 4000.0f, -4000.0f,  4000.0f, -4000.0f, -4000.0f,  4000.0f, -4000.0f,  4000.0f,  4000.0f,  4000.0f,  4000.0f,  4000.0f // +Z
	
	};
	glGenBuffers(1, &_vbo_vertexes);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_vertexes);
	glBufferData(
		GL_ARRAY_BUFFER, 
		sizeof(cube_vertices), 
		cube_vertices, 
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER,0);

}

void Cubemap::render(mat4 &pvm)
{
	//glDisable(GL_DEPTH_TEST);
	glBindTexture (GL_TEXTURE_CUBE_MAP, _textureID);
	glUseProgram(_program);
	//std::cout << "tex" << _textureID << std::endl;
	glUniformMatrix4fv(_uniform_PVM, 1, GL_FALSE, glm::value_ptr(pvm));
	glBindBuffer(GL_ARRAY_BUFFER,_vbo_vertexes);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 12, (void*)0);
	glDrawArrays(GL_QUADS, 0, 24);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_CUBE_MAP);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glEnable(GL_DEPTH_TEST);

}
