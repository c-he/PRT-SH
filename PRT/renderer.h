#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "hdrTextureCube.h"
#include "Scene.h"
#include "Lighting.h"
#include "DiffuseObject.h"
#include "GeneralObject.h"

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

    void Init(const int lightNumber);
    void Render();

    void Setup(Scene* scene, Lighting* light)
    {
        _scene = scene;
        _lighting = light;
    }

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
    Scene* _scene;
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
    void setupGeneralBuffer(int type, vec3 viewDir);
};

#endif
