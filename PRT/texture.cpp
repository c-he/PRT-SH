/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "texture.h"


Texture2D::Texture2D(GLuint index)
        : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT),
          Filter_Min(GL_LINEAR_MIPMAP_LINEAR), Filter_Max(GL_LINEAR), Index(index)
{
    glGenTextures(1, &this->ID);
}

Texture2D::Texture2D(GLuint width, GLuint height, GLint internalFormat, GLuint imageFormat, GLuint index) :
        Width(width), Height(height), Internal_Format(internalFormat), Image_Format(imageFormat),
        Wrap_S(GL_CLAMP_TO_EDGE), Wrap_T(GL_CLAMP_TO_EDGE), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR), Index(index)
{
    glGenTextures(1, &this->ID);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char *data)
{
    this->Width = width;
    this->Height = height;
    // Create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    // Set Texture wrap and filter modes
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    // Unbind texture
    Unbind();
}

void Texture2D::Bind() const
{
    glActiveTexture(GL_TEXTURE0 + Index);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture2D::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Resize(int width_, int height_)
{
    Width = width_;
    Height = height_;

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, Internal_Format, Width, Height, 0, Image_Format, GL_UNSIGNED_BYTE, nullptr);
}

void Texture2D::SetTextureIndex(GLuint index)
{
    Index = index;
}

Texture3D::Texture3D()
        : Width(0), Height(0), Depth(0), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Wrap_R(GL_REPEAT),
          Filter_Min(GL_LINEAR_MIPMAP_LINEAR),
          Filter_Max(GL_LINEAR)
{
    glGenTextures(1, &this->ID);
}

void Texture3D::Generate(GLuint width, GLuint height, GLuint depth, unsigned char *data)
{
    this->Width = width;
    this->Height = height;
    this->Depth = depth;

    glBindTexture(GL_TEXTURE_3D, this->ID);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_3D);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, this->Wrap_R);
    // Unbind texture
    glBindTexture(GL_TEXTURE_3D, 0);
}
