#ifndef CUBEMAP_H_
#define CUBEMAP_H_

#include <string>
#include <vector>

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"

class Cubemap
{
public:
	Cubemap() = default;
	virtual ~Cubemap();

	void init(std::string path);
	void render(glm::mat4& pvm);

	GLuint _textureID;

private:
	// int _vs,_fs;
	// int _program;
	Shader _shader;

	GLuint _VAO;
	GLuint _VBO;

	// GLint _uniform_PVM, _attrib_vertex, _uniform_cubemap;
	// GLuint _texture2D;

	void setup();
	GLuint loadCubeMap(std::vector<std::string> faces, std::string& path);
};


#endif
