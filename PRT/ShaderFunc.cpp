#include "ShaderFunc.h"
#include <GL/glew.h>


char* file_read(string filename)
{
	FILE* in; 
	fopen_s(&in,filename.c_str(),"rb");
	if (in == NULL) return NULL;

	int res_size = BUFSIZ;
	char* res = (char*)malloc(res_size);
	int nb_read_total = 0;
	
	while (!feof(in) && !ferror(in)) 
	{
		if (nb_read_total + BUFSIZ > res_size) 
		{
			if (res_size > 10*1024*1024) 
				break;
			res_size = res_size * 2;
			res = (char*)realloc(res, res_size);
		}
		char* p_res = res + nb_read_total;
		nb_read_total += fread(p_res, 1, BUFSIZ, in);	

	}
  
	fclose(in);
	res = (char*)realloc(res, nb_read_total + 1);
	res[nb_read_total] = '\0';
	return res;
 
}

GLuint create_shader(string filename, GLenum type)
{
	const GLchar* source = file_read(filename);
	if(source == NULL) 
	{
		fprintf(stderr, "Error opening %s: ", filename.c_str()); perror("");
		return 0;
	}
	GLuint res = glCreateShader(type);
	const GLchar* sources[] = 
	{
		"#define lowp   \n"
		"#define mediump\n"
		"#define highp  \n"
		,
		source 
	};
	glShaderSource(res, 2, sources, NULL);
	free((void*)source);

	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) 
	{
	    fprintf(stderr, "%s:", filename.c_str());
		glDeleteShader(res);
		return 0;
	}
	
	return res;
}
