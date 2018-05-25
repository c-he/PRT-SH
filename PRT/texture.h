/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture2D
{
public:
    // Holds the ID of the texture object, used for all texture operations to reference to this particular texture
    GLuint ID;
    GLuint Index;
    // Texture image dimensions
    GLuint Width, Height; // Width and height of loaded image in pixels
    // Texture Format
    GLint Internal_Format; // Format of texture object
    GLuint Image_Format; // Format of loaded image
    // Texture configuration
    GLuint Wrap_S; // Wrapping mode on S axis
    GLuint Wrap_T; // Wrapping mode on T axis
    GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
    GLuint Filter_Max; // Filtering mode if texture pixels > screen pixels
    // Constructor (sets default texture modes)
    Texture2D(GLuint index = 0);

    // Constructor (this constructor is used for frame buffer).
    Texture2D(GLuint width, GLuint height, GLint internalFormat, GLuint imageFormat, GLuint index = 0);

    // Generates texture from image data
    void Generate(GLuint width, GLuint height, unsigned char *data);

    // Binds the texture as the current active GL_TEXTURE_2D texture object
    void Bind() const;

    void Unbind() const;

    void Resize(int width_, int height_);

    void SetTextureIndex(GLuint index);

    // Overloaded operator.
    bool operator==(Texture2D &texture)
    {
        return this->ID == texture.ID;
    }
};

// Texture3D is designed to store cloud textures from .ex5 files.
class Texture3D
{
public:
    // Holds the ID of the texture object, used for all texture operations to reference to this particular texture.
    GLuint ID;
    // Texture image dimensions
    GLuint Width, Height, Depth;
    // Texture configuration
    GLuint Wrap_S; // Wrapping mode on S axis
    GLuint Wrap_T; // Wrapping mode on T axis
    GLuint Wrap_R; // Wrapping mode on R axis
    GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
    GLuint Filter_Max; // Filtering mode if texture pixels > screen pixels

    // Constructor (sets default texture modes)
    Texture3D();

    // Generates texture from image data
    void Generate(GLuint width, GLuint height, GLuint depth, unsigned char *data);
};

#endif
