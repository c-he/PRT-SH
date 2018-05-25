/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include "stb_image.h"

// Instantiate static variables.
std::unordered_map<std::string, Shader>    ResourceManager::Shaders;
std::unordered_map<std::string, Texture2D> ResourceManager::Texture2Ds;
std::unordered_map<std::string, Texture3D> ResourceManager::Texture3Ds;
std::unordered_map<std::string, Model *>   ResourceManager::Models;
std::unordered_map<std::string, GLuint>    ResourceManager::VAOmap;
std::unordered_map<std::string, int>       ResourceManager::VAOSizeMap;
std::unordered_map<std::string, glm::vec3> ResourceManager::modelSizeMap;

Shader ResourceManager::LoadShader(const string &vShaderFile, const string &fShaderFile, const string &gShaderFile,
                                   std::string name, const GLchar **transformFeedbackVaryings,
                                   unsigned int numTransformFeedbackVaryings, bool interleavedTransformFeedbackAttribs)
{
    if (Shaders.find(name) != Shaders.end()) {
        return Shaders[name];
    }
    return Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile, transformFeedbackVaryings,
                                       numTransformFeedbackVaryings, interleavedTransformFeedbackAttribs);
}

Shader& ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

void ResourceManager::Clear()
{
    // (Properly) delete all shaders.
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
}

Shader
ResourceManager::loadShaderFromFile(const string &vShaderFile, const string &fShaderFile, const string &gShaderFile,
                                    const GLchar **transformFeedbackVaryings, unsigned int numTransformFeedbackVaryings,
                                    bool interleavedTransformFeedbackAttribs)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // Open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::stringstream vShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        // If fragment shader path is present, load a fragment shader
        if (!fShaderFile.empty())
        {
            std::ifstream fragmentShaderFile(fShaderFile);
            std::stringstream fShaderStream;
            fShaderStream << fragmentShaderFile.rdbuf();
            fragmentShaderFile.close();
            fragmentCode = fShaderStream.str();
        }
        // If geometry shader path is present, also load a geometry shader
        if (!gShaderFile.empty())
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const string &vShaderCode = vertexCode.c_str();
    const string &fShaderCode = fragmentCode.c_str();
    const string &gShaderCode = geometryCode.c_str();
    // 2. Now create shader object from source code
    Shader shader;
	// std::cout << "Loading [" << vShaderFile << "][" << fShaderFile << "][" << gShaderFile << "]" << std::endl;
    shader.Compile(vShaderCode.c_str(), fShaderFile.empty() ? nullptr : fShaderCode.c_str(),
                   gShaderFile.empty() ? nullptr : gShaderCode.c_str(), transformFeedbackVaryings,
                   numTransformFeedbackVaryings, interleavedTransformFeedbackAttribs);
    return shader;
}


Texture2D ResourceManager::LoadTexture2D(const string &file, GLboolean alpha, std::string name)
{
    if (Texture2Ds.find(name) != Texture2Ds.end()) {
        return Texture2Ds[name];
    }
    return Texture2Ds[name] = loadTexture2DFromFile(file, alpha);
}

Texture3D ResourceManager::LoadTexture3D(const string &file, string name)
{
    if (Texture3Ds.find(name) != Texture3Ds.end()) {
        return Texture3Ds[name];
    }
    return Texture3Ds[name] = loadTexture3DFromFile(file);
}

Texture2D ResourceManager::GetTexture2D(std::string name)
{
    return Texture2Ds[name];
}

Texture3D ResourceManager::GetTexture3D(string name)
{
    return Texture3Ds[name];
}

// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
ResourceManager::ResourceManager()
{}

Texture2D ResourceManager::loadTexture2DFromFile(const string &file, GLboolean alpha)
{
    // Create Texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // Load image
    int width, height;
    unsigned char *image = stbi_load(file.c_str(), &width, &height, 0,
                                     texture.Image_Format == GL_RGBA ? STBI_rgb_alpha : STBI_rgb);
    // Now generate texture
    texture.Generate(width, height, image);
    // And finally free image data
    stbi_image_free(image);
    return texture;
}

Texture3D ResourceManager::loadTexture3DFromFile(const string &file)
{
    // Create Texture object
    Texture3D texture;

    // Load image from .ex5 file.
    FILE *fp = fopen(file.c_str(), "r");

    if (fp == NULL)
    {
        std::cout << "Can not open .ex5 file." << std::endl;
    }

    // Read header.
    int width, height, depth;
    fscanf(fp, "%d", &width);
    fscanf(fp, "%d", &height);
    fscanf(fp, "%d", &depth);

    // Create image.
    unsigned char *image = (unsigned char *) malloc(width * height * depth * 4 * sizeof(unsigned char));

    // Read image.
    int i = 0;
    uint32_t pixel;
    while (fscanf(fp, "%d", &pixel) == 1)
    {
        image[i++] = pixel >> 24;
        image[i++] = pixel >> 16;
        image[i++] = pixel >> 8;
        image[i++] = pixel >> 0;
    }

    // Now generate texture.
    texture.Generate(width, height, depth, image);
    // And finally free image data.
    free(image);
    return texture;
}

void ResourceManager::LoadModel(const std::string objModelFile, std::string name)
{
    if (!Models.count(name))
    {
        Model *m = new Model(objModelFile);
        Models[name] = m;
    }
}

Model *ResourceManager::GetModel(std::string name)
{
    return Models[name];
}

GLuint util::genVAO() { GLuint a; glGenVertexArrays(1, &a); return a; }
GLuint util::genBuf() { GLuint a; glGenBuffers(1, &a); return a; }

