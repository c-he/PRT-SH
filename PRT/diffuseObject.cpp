#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>
#include "diffuseObject.h"

void DiffuseObject::write2Disk(std::string filename)
{
    std::ofstream out(filename);
    int size = _vertices.size() / 3;
    int band2 = _band * _band;

    out << _vertices.size() << std::endl;
    out << _band << std::endl;

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < band2; ++j)
        {
            out << _TransferFunc[i][j].x << ' '
                << _TransferFunc[i][j].y << ' '
                << _TransferFunc[i][j].z << ' ';
        }
        out << std::endl;
    }

    out.close();
    std::cout << "Diffuse object generated." << std::endl;
}

void DiffuseObject::write2Diskbin(std::string filename)
{
    std::ofstream out;
    out.open(filename.c_str(), std::ofstream::binary);
    int size = _vertices.size() / 3;
    int band2 = _band * _band;

    out.write((char *)&size, sizeof(unsigned int));
    out.write((char *)&_band, sizeof(int));
    //out << "band = " _band << std::endl;

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < band2; ++j)
        {
            out.write((char *)&_TransferFunc[i][j].x, sizeof(float));
            out.write((char *)&_TransferFunc[i][j].y, sizeof(float));
            out.write((char *)&_TransferFunc[i][j].z, sizeof(float));
        }
    }

    out.close();
    std::cout << "Diffuse object generated." << std::endl;
}

void DiffuseObject::readFDisk(std::string filename)
{
    std::string transf[3] = {"DU.txt", "DS.txt", "DI.txt"};

    for (int i = 0; i < 3; i++)
        _DTransferFunc[i].clear();

    for (int k = 0; k < 3; k++)
    {
        std::string txtFile = filename + transf[k];
        std::ifstream in(txtFile);
        assert(in);
        int size, band2;

        in >> size >> _band;
        band2 = _band * _band;

        std::cout << "Diffuse object: " << filename << std::endl;
        std::cout << "band = " << _band << std::endl;

        std::vector<glm::vec3> empty(band2, glm::vec3(0.0f));

        for (int i = 0; i < size; ++i)
        {
            _DTransferFunc[k].push_back(empty);

            for (int j = 0; j < band2; ++j)
            {
                in >> _DTransferFunc[k][i][j].x
                    >> _DTransferFunc[k][i][j].y
                    >> _DTransferFunc[k][i][j].z;
            }
        }
        in.close();
    }
}

void DiffuseObject::readFDiskbin(std::string filename)
{
    std::string transf[3] = {"DU.dat", "DS.dat", "DI.dat"};

    for (int i = 0; i < 3; i++)
        _DTransferFunc[i].clear();

    for (int k = 0; k < 3; k++)
    {
        std::string dataFile = filename + transf[k];
        std::ifstream in(dataFile, std::ifstream::binary);
        // Assure data file eist in order to avoid memory leak.
        assert(in);
        unsigned int size, band2;

        in.read((char *)&size, sizeof(unsigned int));
        in.read((char *)&_band, sizeof(int));

        std::cout << "Diffuse object: " << filename + transf[k] << std::endl;
        std::cout << "band = " << _band << std::endl;

        band2 = _band * _band;

        std::vector<glm::vec3> empty(band2, glm::vec3(0.0f));

        for (size_t i = 0; i < size; ++i)
        {
            _DTransferFunc[k].push_back(empty);

            for (size_t j = 0; j < band2; ++j)
            {
                in.read((char *)&_DTransferFunc[k][i][j].x, sizeof(float));
                in.read((char *)&_DTransferFunc[k][i][j].y, sizeof(float));
                in.read((char *)&_DTransferFunc[k][i][j].z, sizeof(float));
            }
        }
        in.close();
    }
}

void DiffuseObject::diffuseUnshadow(int size, int band2, Sampler* sampler, TransferType type, BVHTree* Inbvht)
{
    bool shadow = false;
    if (type != T_UNSHADOW)
    {
        shadow = true;
    }

    bool visibility;

    std::vector<glm::vec3> empty(band2, glm::vec3(0.0f));
    _TransferFunc.resize(size, empty);

    // Build BVH.
    BVHTree bvht;
    if (shadow)
    {
        if (type == T_SHADOW)
            bvht.build(*this);
        else
            bvht = *Inbvht;
    }

    // Sample.
    const int sampleNumber = sampler->_samples.size();

#pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        int index = 3 * i;

        glm::vec3 normal(_normals[index + 0], _normals[index + 1], _normals[index + 2]);

        for (int j = 0; j < sampleNumber; j++)
        {
            Sample stemp = sampler->_samples[j];
            float H = std::max(glm::dot(glm::normalize(normal), glm::normalize(stemp._cartesCoord)), 0.0f);

            // Shadow.
            if (shadow)
            {
                Ray testRay(glm::vec3(_vertices[index + 0], _vertices[index + 1], _vertices[index + 2]),
                            stemp._cartesCoord);
                visibility = !bvht.intersect(testRay);
            }
            else
            {
                visibility = true;
            }

            if (!visibility)
            {
                H = 0.0f;
            }
            // Projection.
            for (int k = 0; k < band2; k++)
            {
                float SHvalue = stemp._SHvalue[k];

                _TransferFunc[i][k] += _albedo * SHvalue * H;
            }
        }
    }
    // Normalization.
    float weight = 4.0f * M_PI / sampler->_samples.size();
#pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < band2; ++j)
        {
            _TransferFunc[i][j] *= weight;
        }
    }
    if (type == T_UNSHADOW)
        std::cout << "Unshadowed transfer vector generated." << std::endl;
}

void DiffuseObject::diffuseShadow(int size, int band2, Sampler* sampler, TransferType type, BVHTree* Inbvht)
{
    diffuseUnshadow(size, band2, sampler, type, Inbvht);
    if (type == T_SHADOW)
        std::cout << "Shadowed transfer vector generated." << std::endl;
    //system("pause");
}

void DiffuseObject::diffuseInterreflect(int size, int band2, Sampler* sampler, TransferType type, int bounce)
{
    BVHTree bvht;
    bvht.build(*this);

    diffuseShadow(size, band2, sampler, type, &bvht);

    const int sampleNumber = sampler->_samples.size();

    auto interReflect = new std::vector<std::vector<glm::vec3>>[bounce + 1];

    interReflect[0] = _TransferFunc;
    std::vector<glm::vec3> empty(band2, glm::vec3(0.0f));

    float weight = 4.0f * M_PI / sampleNumber;

    for (int k = 0; k < bounce; k++)
    {
        std::vector<std::vector<glm::vec3>> zeroVector(size, empty);
        interReflect[k + 1].resize(size);

#pragma omp parallel for
        for (int i = 0; i < size; i++)
        {
            int offset = 3 * i;
            glm::vec3 normal = glm::vec3(_normals[offset + 0], _normals[offset + 1], _normals[offset + 2]);

            for (int j = 0; j < sampleNumber; j++)
            {
                Sample stemp = sampler->_samples[j];
                Ray rtemp(glm::vec3(_vertices[offset + 0], _vertices[offset + 1], _vertices[offset + 2]),
                          stemp._cartesCoord);

                bool visibility = !bvht.intersect(rtemp);
                if (visibility)
                    continue;
                // The direction which is invisibile is where the indirect radiance comes from.
                float H = std::max(glm::dot(rtemp._dir, normal), 0.0f);

                int triIndex = 3 * rtemp._index;
                int voffset[3];
                glm::vec3 p[3];
                std::vector<glm::vec3>* SHTrans[3];
                for (int m = 0; m < 3; m++)
                {
                    voffset[m] = _indices[triIndex + m];
                    SHTrans[m] = &interReflect[k][voffset[m]];
                    voffset[m] *= 3;
                    p[m] = glm::vec3(_vertices[voffset[m] + 0], _vertices[voffset[m] + 1], _vertices[voffset[m] + 2]);
                }
                glm::vec3 pc = rtemp._o + (float)rtemp._t * rtemp._dir;

                float u, v, w;
                // Barycentric coordinates for interpolation.
                barycentric(pc, p, u, v, w);

                std::vector<glm::vec3> SHtemp;
                SHtemp.resize(band2);

                for (int m = 0; m < band2; m++)
                {
                    SHtemp[m] = u * SHTrans[0]->at(m) + v * SHTrans[1]->at(m) + w * SHTrans[2]->at(m);
                    zeroVector[i][m] += H * _albedo * SHtemp[m];
                }
            }
        }

#pragma omp parallel for
        for (int i = 0; i < size; ++i)
        {
            interReflect[k + 1][i].resize(band2);
            for (int j = 0; j < band2; ++j)
            {
                zeroVector[i][j] *= weight;
                interReflect[k + 1][i][j] = interReflect[k][i][j] + zeroVector[i][j];
            }
        }
    }
    _TransferFunc = interReflect[bounce];
    delete[] interReflect;
    std::cout << "Interreflected transfer vector generated." << std::endl;
}

void DiffuseObject::project2SH(int mode, int band, int sampleNumber, int bounce)
{
    _band = band;

    int size = _vertices.size() / 3;
    int band2 = band * band;

    Sampler stemp((unsigned)sqrt(sampleNumber));
    stemp.computeSH(band);

    if (mode == 1)
    {
        std::cout << "Transfer Type: unshadowed" << std::endl;
        diffuseUnshadow(size, band2, &stemp, T_UNSHADOW);
    }
    else if (mode == 2)
    {
        std::cout << "Transfer Type: shadowed" << std::endl;
        diffuseShadow(size, band2, &stemp, T_SHADOW);
    }
    else if (mode == 3)
    {
        std::cout << "Transfer Type: interreflect" << std::endl;
        diffuseInterreflect(size, band2, &stemp, T_INTERREFLECT, bounce);
    }
}
