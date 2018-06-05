#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "hdrTextureCube.h"
#include "lighting.h"
#include "diffuseObject.h"
#include "generalObject.h"

// Camera intrinsic parameters definetion.
#define ZOOM 45.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 10000.0f

struct MeshVertex
{
    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
};

class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer();

    void Init(int lightNumber);
    void Render();

    void Setup(DiffuseObject* obj, Lighting* light)
    {
        _diffObject = obj;
        _lighting = light;
        _genObject = nullptr;
    }

    void Setup(GeneralObject* obj, Lighting* light)
    {
        _genObject = obj;
        _lighting = light;
        _diffObject = nullptr;
    }

    void SetupColorBuffer(int type, glm::vec3 viewDir, bool diffuse = true);

private:
    DiffuseObject* _diffObject;
    GeneralObject* _genObject;

    Lighting* _lighting;
    std::vector<float> _colorBuffer;
    std::vector<MeshVertex> _meshBuffer;

    HDRTextureCube* hdrTextures;
    glm::mat4 projection;

    GLuint _VAO;
    GLuint _VBO;

    void objDraw();
    void setupDiffuseBuffer(int type);
    void setupGeneralBuffer(int type, glm::vec3 viewDir);
};

#endif
