#ifndef SHADERFUNC_H_
#define SHADERFUNC_H_

#include <cstdio>
#include <cstdlib>
#include <string>
#include "GL/glew.h"

using std::string;

GLuint create_shader(string filename, GLenum type);

#endif
