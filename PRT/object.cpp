#include "object.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

void Object::normVertices()
{
    int size = _vertices.size();
    float scaleX = std::max(fabs(_vmaxX), fabs(_vminX));
    float scaleY = std::max(fabs(_vmaxY), fabs(_vminY));
    float scaleZ = std::max(fabs(_vmaxZ), fabs(_vminZ));

    float weight = 1.0f / Trimax(scaleX, scaleY, scaleZ);

    for (int i = 0; i < size; ++i)
    {
        _vertices[i] *= weight;
    }
}

void Object::init(std::string path, glm::vec3 albedo, bool texture)
{
    _modelname = path;
    _albedo = albedo;

    std::ifstream in;
    try
    {
        in.open(path);
    }
    catch (...)
    {
        std::cout << "Scene loaded error" << std::endl;
    }
    if (!in.is_open())
    {
        std::cout << "Obj not opened!" << std::endl;
        exit(0);
    }
    std::string line, attribute;

    float x, y, z, u, v, nx, ny, nz;
    char mask;
    unsigned index_v0, index_v1, index_v2;
    unsigned index_n0, index_n1, index_n2;
    unsigned index_t0, index_t1, index_t2;

    _vmaxX = _vmaxY = _vmaxZ = -FLT_MAX;
    _vminX = _vminY = _vminZ = FLT_MAX;

    while (getline(in, line))
    {
        std::istringstream s_line(line);
        s_line >> attribute;
        if (attribute == "#")
            continue;

        if (attribute == "v")
        {
            s_line >> x >> y >> z;

            if (x > _vmaxX)_vmaxX = x;
            if (x < _vminX)_vminX = x;
            if (y > _vmaxY)_vmaxY = y;
            if (y < _vminY)_vminY = y;
            if (z > _vmaxZ)_vmaxZ = z;
            if (z < _vminZ)_vminZ = z;

            _vertices.push_back(x);
            _vertices.push_back(y);
            _vertices.push_back(z);
        }
        else if (attribute == "vt")
        {
            s_line >> u >> v;
            _texcoords.push_back(u);
            _texcoords.push_back(v);
        }
        else if (attribute == "vn")
        {
            s_line >> nx >> ny >> nz;
            _normals.push_back(nx);
            _normals.push_back(ny);
            _normals.push_back(nz);
        }
        else if (attribute == "f")
        {
            if (texture)
            {
                s_line >> index_v0 >> mask >> index_n0 >> mask >> index_t0
                    >> index_v1 >> mask >> index_n1 >> mask >> index_t1
                    >> index_v2 >> mask >> index_n2 >> mask >> index_t2;
            }
            else
            {
                s_line >> index_v0 >> mask >> mask >> index_n0
                    >> index_v1 >> mask >> mask >> index_n1
                    >> index_v2 >> mask >> mask >> index_n2;
            }

            _indices.push_back(index_v0 - 1);
            _indices.push_back(index_v1 - 1);
            _indices.push_back(index_v2 - 1);
        }
    }
    in.close();

    normVertices();
}


bool Object::intersectTest(Ray& ray, int facenumber)
{
    bool result = false;

    // Naive approach O(n).
    for (int i = 0; i < facenumber; i++)
    {
        int offset = 3 * i;
        int index[3];
        index[0] = _indices[offset + 0];
        index[1] = _indices[offset + 1];
        index[2] = _indices[offset + 2];

        glm::vec3 p[3];
        for (int j = 0; j < 3; j++)
        {
            int Vindex = 3 * index[j];
            p[j] = glm::vec3(_vertices[Vindex], _vertices[Vindex + 1], _vertices[Vindex + 2]);
        }

        Triangle Ttemp(p[0], p[1], p[2], i);

        if (rayTriangle(ray, Ttemp))
        {
            result = true;
            break;
        }
    }

    return result;
}
