#include <iostream>
#include "hdrTextureCube.h"
#include "rgbe.h"
#include "resource_manager.h"

void HDRTextureCube::Init(std::string path)
{
    Format = GL_RGB;
    Type = GL_FLOAT;
    InternalFormat = GL_RGB16F_ARB;

    GLfloat vertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glBindVertexArray(0);

    // Shader configuration.
    shader = ResourceManager::GetShader("cubemap");
    shader.Use();
    shader.SetInteger("cubemap", 0);

    // Load textures.
    loadHDR(path);

    // Generate cube map textures.
    generate();
}

HDRTextureCube::~HDRTextureCube()
{
    glDeleteTextures(1, &ID);
}

void HDRTextureCube::loadHDR(std::string& path)
{
    std::cout << "Loading HDR texture: " << path << std::endl;

    FILE* file = fopen(path.c_str(), "rb");
    RGBE_ReadHeader(file, &width, &height, NULL);
    data = new float[3 * width * height];
    RGBE_ReadPixels_RLE(file, data, width, height);

    fclose(file);

    std::cout << " - WIDTH                    : " << width << std::endl;
    std::cout << " - HEIGHT                   : " << height << std::endl;
    std::cout << " - MEMORY SIZE              : " << (3 * width * height * sizeof(float)) / 8 << " bytes" << std::endl;
    std::cout << " - FORMAT                   : " << ((Format == GL_RGB) ? "RGB" : "") << std::endl;
    std::cout << " - TYPE                     : " << ((Type == GL_FLOAT) ? "Float" : "") << std::endl;
    std::cout << " - INTERNAL FORMAT          : " << ((InternalFormat == GL_RGB8) ? "RGB8" : "RGB16F") << std::endl;

    loadFaces();
}

void HDRTextureCube::loadFaces()
{
    faces = new Face *[6];

    int faceWidth = width / 3;
    int faceHeight = height / 4;

    for (int i = 0; i < 6; ++i)
    {
        faces[i] = new Face();
        faces[i]->data = new float[3 * faceWidth * faceHeight];
        faces[i]->width = faceWidth;
        faces[i]->height = faceHeight;
        faces[i]->currentOffset = 0;
    }

    for (int l = 0; l < height; ++l)
    {
        int jFace = (l - (l % faceHeight)) / faceHeight;

        for (int iFace = 0; iFace < 3; ++iFace)
        {
            Face* face = NULL;
            int offset = 3 * (faceWidth * iFace + l * width);

            if (iFace == 2 && jFace == 1) face = faces[0]; // POS_Y
            if (iFace == 0 && jFace == 1) face = faces[1]; // NEG_Y
            if (iFace == 1 && jFace == 0) face = faces[2]; // POS_X
            if (iFace == 1 && jFace == 2) face = faces[3]; // NEG_X
            if (iFace == 1 && jFace == 1) face = faces[4]; // POS_Z
            if (iFace == 1 && jFace == 3) face = faces[5]; // NEG_Z

            if (face)
            {
                // the number of components to copy
                int n = sizeof(float) * faceWidth * 3;

                memcpy(face->data + face->currentOffset, data + offset, n);
                face->currentOffset += (3 * faceWidth);
            }
        }
    }

    // adjust NEG_Z face
    flipHorizontal(faces[5]);
    flipVertical(faces[5]);
}

void HDRTextureCube::flipHorizontal(Face* face)
{
    int dataSize = face->width * face->height * 3;
    int n = sizeof(float) * 3 * face->width;
    float* newData = new float[dataSize];

    for (int i = 0; i < face->height; i++)
    {
        int offset = i * face->width * 3;
        int bias = -(i + 1) * 3 * face->width;

        memcpy(newData + dataSize + bias, face->data + offset, n);
    }

    delete[] face->data;
    face->data = newData;
}

void HDRTextureCube::flipVertical(Face* face)
{
    int dataSize = face->width * face->height * 3;
    int n = sizeof(float) * 3;
    float* newData = new float[dataSize];

    for (int i = 0; i < face->height; ++i)
    {
        int lineOffset = i * face->width * 3;

        for (int j = 0; j < face->width; ++j)
        {
            int offset = lineOffset + j * 3;
            int bias = lineOffset + face->width * 3 - (j + 1) * 3;

            memcpy(newData + bias, face->data + offset, n);
        }
    }

    delete[] face->data;
    face->data = newData;
}

void HDRTextureCube::generate()
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    for (int i = 0; i < 6; ++i)
    {
        Face* f = faces[i];
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F_ARB, f->width, f->height, 0, GL_RGB, GL_FLOAT,
                     f->data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void HDRTextureCube::Bind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void HDRTextureCube::Draw()
{
    // std::cout << "texture: " << ID << std::endl;
    // Change depth function so depth test passes when values are equal to depth buffer's content.
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(VAO);
    Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // Unbind.
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    // Set depth function back to default.
    glDepthFunc(GL_LESS);
}
