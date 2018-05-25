/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "texture.h"

using std::string;

// General purpsoe shader object. Compiles from file, generates
// compile/link-time error messages and hosts several utility
// functions for easy management.
class Shader
{
public:
    // State
    GLuint ID;

    // Constructor
    Shader()
    {}

    // Sets the current shader as active
    Shader &Use();

    // Compiles the shader from given source code
    void Compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr,
                 const GLchar **transformFeedbackVaryings = nullptr, unsigned int numTransformFeedbackVaryings = 0,
                 bool interleavedTransformFeedbackAttribs = true); // Note: geometry source code is optional
    // Utility functions
    void SetFloat(const string &name, GLfloat value, GLboolean useShader = false);

    void SetInteger(const string &name, GLint value, GLboolean useShader = false);

    void SetBoolean(const string &name, GLboolean value, GLboolean useShader = false);

    void SetVector2f(const string &name, GLfloat x, GLfloat y, GLboolean useShader = false);

    void SetVector2f(const string &name, const glm::vec2 &value, GLboolean useShader = false);

    void SetVector3f(const string &name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);

    void SetVector3f(const string &name, const glm::vec3 &value, GLboolean useShader = false);

    void SetVector4f(const string &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);

    void SetVector4f(const string &name, const glm::vec4 &value, GLboolean useShader = false);

    void SetMatrix4(const string &name, const glm::mat4 &matrix, GLboolean useShader = false);

    void SetTexture(const string &name, const Texture2D *texture, GLboolean useShader = false);

private:
    // Checks if compilation or linking failed and if so, print the error logs
    void checkCompileErrors(GLuint object, string type);
};

#endif
