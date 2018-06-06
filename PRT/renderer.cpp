#include <cmath>
#include "renderer.h"
#include "UI.h"
#include "resource_manager.h"
#include "sphericalHarmonics.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern bool drawCubemap;
extern bool simpleLight;

extern std::string lightings[];
extern int lightingIndex;
extern int objectIndex;
extern int bandIndex;

// Window.
extern int WIDTH;
extern int HEIGHT;

// Camera.
extern float camera_dis;
extern glm::vec3 camera_pos;
extern glm::vec3 last_camera_pos;
extern glm::vec3 camera_dir;
extern glm::vec3 camera_up;

// Rotation.
extern int g_AutoRotate;
extern int g_RotateTime;
extern glm::fquat last_Rotation;
extern glm::mat4 rotateMatrix;

extern DiffuseObject** diffObject;
extern GeneralObject* genObject;
extern Lighting** lighting;
extern Lighting* simpleL;

// Mesh information.
int vertices;
int faces;

Renderer::~Renderer()
{
    delete[]hdrTextures;
}

void Renderer::Init(const int lightNumber)
{
    // Initialize cubemap.
    hdrTextures = new HDRTextureCube[lightNumber];
    for (size_t i = 0; i < lightNumber; ++i)
    {
        hdrTextures[i].Init("lightings/cross/" + lightings[i] + "_cross" + ".hdr");
    }

    // Initialize projection matrix.
    projection = glm::perspective(ZOOM, (float)WIDTH / (float)HEIGHT, NEAR_PLANE, FAR_PLANE);
}

void Renderer::SetupColorBuffer(int type, glm::vec3 viewDir, bool diffuse)
{
    if (diffuse)
        setupDiffuseBuffer(type);
    else
        setupGeneralBuffer(type, viewDir);
}

void Renderer::setupDiffuseBuffer(int type)
{
    assert(_diffObject->band() == _lighting->band());
    int vertexnumber = _diffObject->_vertices.size() / 3;
    int band2 = _diffObject->band() * _diffObject->band();

    // Generate color buffer.
    _colorBuffer.clear();
    for (int i = 0; i < vertexnumber; ++i)
    {
        float cr = 0.0f, cg = 0.0f, cb = 0.0f;
        glm::vec3 lightcoeff;
        for (int j = 0; j < band2; ++j)
        {
            if (simpleLight)
            {
                lightcoeff.r = simpleL[bandIndex]._Vcoeffs[0](j);
                lightcoeff.g = simpleL[bandIndex]._Vcoeffs[1](j);
                lightcoeff.b = simpleL[bandIndex]._Vcoeffs[2](j);
            }
            else
            {
                lightcoeff.r = _lighting->_Vcoeffs[0](j);
                lightcoeff.g = _lighting->_Vcoeffs[1](j);
                lightcoeff.b = _lighting->_Vcoeffs[2](j);
            }

            cr += lightcoeff.r * _diffObject->_DTransferFunc[type][i][j].r;
            cg += lightcoeff.g * _diffObject->_DTransferFunc[type][i][j].g;
            cb += lightcoeff.b * _diffObject->_DTransferFunc[type][i][j].b;
        }

        cr *= _lighting->hdrEffect().r;
        cg *= _lighting->hdrEffect().g;
        cb *= _lighting->hdrEffect().b;

        _colorBuffer.push_back(cr);
        _colorBuffer.push_back(cg);
        _colorBuffer.push_back(cb);
    }
    // Generate mesh buffer.
    _meshBuffer.clear();
    int facenumber = _diffObject->_indices.size() / 3;
    for (int i = 0; i < facenumber; i++)
    {
        int offset = 3 * i;
        int index[3] = {
            _diffObject->_indices[offset + 0],
            _diffObject->_indices[offset + 1],
            _diffObject->_indices[offset + 2]
        };

        for (int j = 0; j < 3; j++)
        {
            int Vindex = 3 * index[j];
            MeshVertex vertex = {
                _diffObject->_vertices[Vindex + 0],
                _diffObject->_vertices[Vindex + 1],
                _diffObject->_vertices[Vindex + 2],
                _colorBuffer[Vindex + 0],
                _colorBuffer[Vindex + 1],
                _colorBuffer[Vindex + 2]
            };
            _meshBuffer.push_back(vertex);
        }
    }

    // Set the objects we need in the rendering process (namely, VAO, VBO and EBO).
    // Prevent redundant VAO & VBO generation.
    if (!_VAO)
    {
        glGenVertexArrays(1, &_VAO);
    }
    if (!_VBO)
    {
        glGenBuffers(1, &_VBO);
    }
    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, _meshBuffer.size() * sizeof(MeshVertex), &(_meshBuffer[0]), GL_STATIC_DRAW);

    // Position attribute.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)0);
    // Color attribute.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)(3 * sizeof(float)));

    // Unbind.
    glBindVertexArray(0);
}

void Renderer::setupGeneralBuffer(int type, glm::vec3 viewDir)
{
    assert(_genObject->band() == _lighting->band());
    int vertexnumber = _genObject->_vertices.size() / 3;
    int band = _genObject->band();

    // Generate color buffer.
    _colorBuffer.clear();
    _colorBuffer.resize(_genObject->_vertices.size());

#pragma omp parallel for
    for (int i = 0; i < vertexnumber; i++)
    {
        int offset = 3 * i;
        glm::vec3 normal(_genObject->_normals[offset], _genObject->_normals[offset + 1],
                         _genObject->_normals[offset + 2]);

        float color[3];
        // Matrix multiplication.
        Eigen::VectorXf transferedLight[3];
        transferedLight[0] = _genObject->_TransferMatrix[type][i] * _lighting->_Vcoeffs[0];
        transferedLight[1] = _genObject->_TransferMatrix[type][i] * _lighting->_Vcoeffs[1];
        transferedLight[2] = _genObject->_TransferMatrix[type][i] * _lighting->_Vcoeffs[2];

        Lighting lightingtemp(band, transferedLight);

        float rotateMatrix[3][3];
        glm::vec3 tangent(_genObject->_tangent[i].x, _genObject->_tangent[i].y, _genObject->_tangent[i].z);
        glm::vec3 binormal = glm::cross(normal, tangent) * _genObject->_tangent[i].w;

        for (int m = 0; m < 3; m++)
            rotateMatrix[m][0] = tangent[m];
        for (int m = 0; m < 3; m++)
            rotateMatrix[m][1] = binormal[m];
        for (int m = 0; m < 3; m++)
            rotateMatrix[m][2] = normal[m];

        float alpha, beta, gamma;

        rotateMatrixtoZYZ(rotateMatrix, alpha, beta, gamma);

        std::vector<glm::vec2> paraResult;
        paraResult.emplace_back(glm::vec2(gamma, beta));
        paraResult.emplace_back(glm::vec2(alpha, 0.0f));

        lightingtemp.rotateZYZ(paraResult);

        // CONVOLUTION wih BRDF.
        for (int l = 0; l < band; l++)
        {
            float alpha_l_0 = sqrt((4.0f * M_PI) / (2 * l + 1));

            int BRDFindex = l * (l + 1);
            for (int m = -l; m <= l; m++)
            {
                int index = BRDFindex + m;
                for (int k = 0; k < 3; k++)
                {
                    lightingtemp._Vcoeffs[k](index) *= (alpha_l_0 * _genObject->_BRDFcoeff(BRDFindex));
                }
            }
        }

        // Calculate R in Cartesian Coordinates.
        glm::vec3 N = glm::normalize(normal);
        glm::vec3 V = glm::normalize(viewDir);
        glm::vec3 R = 2 * glm::dot(N, V) * N - V;
        // Convert R to spherical coordinates.
        float theta = acos(R.z);
        float phi = inverseSC(R.y / sin(theta), R.x / sin(theta));
        // Evaluate at the view-dependent reflection direction R.
        for (int s = 0; s < 3; s++)
        {
            color[s] = 0.0f;
            for (int l = 0; l < band; l++)
            {
                for (int m = -l; m <= l; m++)
                {
                    int index = l * (l + 1) + m;
                    color[s] += lightingtemp._Vcoeffs[s](index) * (float)SphericalH::SHvalue(theta, phi, l, m);
                }
            }

            color[s] *= _lighting->glossyEffect()[s];
            _colorBuffer[3 * i + s] = color[s];
        }
    }
    // Generate mesh buffer.
    _meshBuffer.clear();
    int facenumber = _genObject->_indices.size() / 3;
    for (int i = 0; i < facenumber; i++)
    {
        int offset = 3 * i;
        int index[3] = {
            _genObject->_indices[offset + 0],
            _genObject->_indices[offset + 1],
            _genObject->_indices[offset + 2]
        };

        for (int j = 0; j < 3; j++)
        {
            int Vindex = 3 * index[j];
            MeshVertex vertex = {
                _genObject->_vertices[Vindex + 0],
                _genObject->_vertices[Vindex + 1],
                _genObject->_vertices[Vindex + 2],
                _colorBuffer[Vindex + 0],
                _colorBuffer[Vindex + 1],
                _colorBuffer[Vindex + 2]
            };
            _meshBuffer.push_back(vertex);
        }
    }

    // Set the objects we need in the rendering process (namely, VAO, VBO and EBO).
    if (!_VAO)
    {
        glGenVertexArrays(1, &_VAO);
    }
    if (!_VBO)
    {
        glGenBuffers(1, &_VBO);
    }
    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, _meshBuffer.size() * sizeof(MeshVertex), &(_meshBuffer[0]), GL_STATIC_DRAW);

    // Position attribute.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)0);
    // Color attribute.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)(3 * sizeof(float)));

    // Unbind.
    glBindVertexArray(0);
}

void Renderer::objDraw()
{
    glBindVertexArray(_VAO);
    if (_genObject == nullptr)
    {
        vertices = _diffObject->_vertices.size() / 3;
        faces = _diffObject->_indices.size() / 3;
    }
    else if (_diffObject == nullptr)
    {
        vertices = _genObject->_vertices.size() / 3;
        faces = _genObject->_indices.size() / 3;
    }

    glDrawArrays(GL_TRIANGLES, 0, _meshBuffer.size());

    // Unbind.
    glBindVertexArray(0);
}

void Renderer::Render()
{
    // Render objects.
    glm::mat4 view = glm::lookAt(camera_dis * camera_pos, camera_dir, camera_up);
    glm::mat4 model;
    bool b_rotate = false;
    if (g_AutoRotate)
    {
        glm::vec3 axis(0.0f, 1.0f, 0.0f);
        float angle = glfwGetTime() - g_RotateTime;
        // std::cout << "angle = " << angle << std::endl;
        glm::fquat quat = glm::angleAxis(angle, axis);
        g_Rotation = g_RotateStart * quat;
        b_rotate = true;
    }
    if (g_Rotation != last_Rotation)
    {
        b_rotate = true;
        last_Rotation = g_Rotation;
    }
    rotateMatrix = glm::mat4_cast(g_Rotation);
    model = rotateMatrix * model;
    Shader shader = ResourceManager::GetShader("prt");
    shader.Use();
    shader.SetMatrix4("model", model);
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);

    // rotate vector = [sin(theta)cos(phi), sin(theta)sin(phi), cos(theta)]
    glm::vec3 rotateVector;
    bool b_rotateLight = false;
    float thetatemp;
    float phitemp;
    if (b_rotate || simpleLight)
    {
        glm::vec4 dir = rotateMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
        rotateVector = glm::vec3(dir.x, dir.y, dir.z);
        rotateVector = glm::clamp(rotateVector, -1.0f, 1.0f);
        thetatemp = acos(rotateVector.z);
        if (dir.x < 0)
        {
            thetatemp = 2.0f * M_PI - thetatemp;
        }
        phitemp = inverseSC(rotateVector.y / sin(thetatemp), rotateVector.x / sin(thetatemp));
        b_rotateLight = true;
    }
    if (simpleLight && !b_rotate)
    {
        rotateVector = light_dir;
        b_rotateLight = true;
    }

    if (b_rotateLight)
    {
        rotateVector = glm::normalize(rotateVector);
        float theta = acos(rotateVector.z);
        float phi;
        if (fabs(sin(theta)) < M_ZERO)
        {
            phi = 0.0f;
        }
        else
        {
            phi = inverseSC(rotateVector.y / sin(theta), rotateVector.x / sin(theta));
        }
        std::vector<glm::vec2> rotatePara;
        rotatePara.clear();

        if (simpleLight && !b_rotate)
        {
            rotatePara.emplace_back(glm::vec2(theta, phi));
            simpleL[bandIndex].rotateZYZ(rotatePara);
        }
        if (b_rotate)
        {
            rotatePara.emplace_back(glm::vec2(-thetatemp, -phitemp));
            if (simpleLight)
            {
                simpleL[bandIndex].rotateZYZ(rotatePara);
            }
            else
            {
                lighting[lightingIndex][bandIndex].rotateZYZ(rotatePara);
            }
        }

        if (materialIndex == 0)
        {
            Setup(&diffObject[objectIndex][bandIndex], &lighting[lightingIndex][bandIndex]);
            SetupColorBuffer(transferFIndex, glm::vec3(0.0f, 0.0f, 0.0f), true);
        }
        else
        {
            Setup(&genObject[objectIndex], &lighting[lightingIndex][bandIndex]);
            SetupColorBuffer(transferFIndex, camera_dis * camera_pos, false);
        }
    }

    if (materialIndex == 1)
    {
        if (glm::any(glm::notEqual(camera_pos, last_camera_pos)))
        {
            Setup(&genObject[objectIndex], &lighting[lightingIndex][bandIndex]);
            SetupColorBuffer(transferFIndex, camera_dis * camera_pos, false);
            last_camera_pos = camera_pos;
        }
    }

    objDraw();

    if (drawCubemap)
    {
        // Render cubemap.
        shader = ResourceManager::GetShader("cubemap");
        shader.Use();
        // Remove translation from the view matrix.
        view = glm::mat4(glm::mat3(view));
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("projection", projection);
        hdrTextures[lightingIndex].Draw();
    }
}
