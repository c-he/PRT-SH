#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <unordered_map>
#include <string>

#include <GL/glew.h>
#include "texture.h"
#include "shader.h"
#include "model.h"
#ifdef _WIN32
#define _SHADER_PREFIX_ "shaders"
#define _TEXTURE_PREFIX_ "textures"
#define _MODEL_PREFIX_ "models"
#else
#define _SHADER_PREFIX_ "../shaders"
#define _TEXTURE_PREFIX_ "../textures"
#define _MODEL_PREFIX_ "../models"
#endif
using std::string;

// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no
// public constructor is defined.
class ResourceManager
{
public:
    // Resource storage
    static std::unordered_map<string, Shader> Shaders;
    static std::unordered_map<string, Texture2D> Texture2Ds;
    static std::unordered_map<string, Texture3D> Texture3Ds;
    static std::unordered_map<string, Model *> Models;
    static std::unordered_map<string, GLuint> VAOmap;
    static std::unordered_map<string, int> VAOSizeMap;
    static std::unordered_map<string, glm::vec3> modelSizeMap;

    // Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader.
    static Shader
    LoadShader(const string &vShaderFile, const string &fShaderFile, const string &gShaderFile, string name,
               const GLchar **transformFeedbackVaryings = nullptr, unsigned int numTransformFeedbackVaryings = 0,
               bool interleavedTransformFeedbackAttribs = true);

    // Retrieves a stored shader.
    static Shader& GetShader(string name);

    // Properly de-allocates all loaded resources.
    static void Clear();

    static Texture2D LoadTexture2D(const string &file, GLboolean alpha, string name);

    static Texture3D LoadTexture3D(const string &file, string name);

    // Retrieves a stored texture.
    static Texture2D GetTexture2D(string name);

    static Texture3D GetTexture3D(string name);

    // Load a model from a .obj file.
    static void LoadModel(string objModelFile, string name);

    // Retrieves a stored model.
    static Model *GetModel(string name);

private:
    // Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager();

    // Loads and generates a shader from file
    static Shader
    loadShaderFromFile(const string &vShaderFile, const string &fShaderFile, const string &gShaderFile = nullptr,
                       const GLchar **transformFeedbackVaryings = nullptr,
                       unsigned int numTransformFeedbackVaryings = 0, bool interleavedTransformFeedbackAttribs = true);

    static Texture2D loadTexture2DFromFile(const string &file, GLboolean alpha);

    static Texture3D loadTexture3DFromFile(const string &file);
};

namespace util {
	GLuint genVAO(); 
	GLuint genBuf();
}
#endif