#ifndef _HDRTEXTURECUBE_H
#define _HDRTEXTURECUBE_H

#include <string>
#include <GL/glew.h>
#include "shader.h"

struct Face
{
    float* data;
    int width;
    int height;
    // for mem copy purposes only
    int currentOffset;
};

const GLenum CubeMapFace[6]{
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

class HDRTextureCube
{
public:
    HDRTextureCube() = default;

    ~HDRTextureCube();

    void Init(std::string path);

    void Bind();

    void Draw();

private:
    void loadHDR(std::string& path);

    void loadFaces();

    void flipHorizontal(Face* faces);

    void flipVertical(Face* faces);

    void generate();

    // Texture attributes.
    GLuint ID;
    GLuint Format;
    GLuint Type;
    GLuint InternalFormat;

    // Shader attributes.
    Shader shader;
    GLuint VAO, VBO;

    float* data;
    int width;
    int height;
    Face** faces;
};

#endif
