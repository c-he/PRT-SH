#ifndef OBJECT_H_
#define OBJECT_H_

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "utils.h"

enum TransferType
{
    T_UNSHADOW,
    T_SHADOW,
    T_INTERREFLECT
};

struct Intersection
{
    bool _intersect;
    Ray _ray;

    Intersection()
    {
        _intersect = false;
    }

    Intersection(bool in, Ray rayin)
        : _intersect(in), _ray(rayin)
    {
    }
};

class Object
{
public:
    Object():
        _theta(0.0f),
        _rx(0.0f),
        _ry(0.0f),
        _rz(1.0f),
        _difforGeneral(false)
    {
    }

    void init(std::string path, glm::vec3 albedo, bool texture = true);

    // Project to SH function.
    virtual void project2SH(int mode, int band, int sampleNumber, int bounce) = 0;
    // IO functions.
    virtual void write2Disk(std::string filename) = 0;
    virtual void write2Diskbin(std::string filename) = 0;
    virtual void readFDisk(std::string filename) = 0;
    virtual void readFDiskbin(std::string filename) = 0;
    bool intersectTest(Ray& ray, int facenumber);
    void normVertices();

    void setRotate(float theta, float x, float y, float z)
    {
        _theta = theta;
        _rx = x;
        _ry = y;
        _rz = z;
    }

    int band() { return _band; }

protected:
    float _vmaxX, _vmaxY, _vmaxZ;
    float _vminX, _vminY, _vminZ;

    glm::vec3 _albedo;
    int _band;

    bool _difforGeneral; //false means diffuse

public:
    std::vector<float> _vertices;
    std::vector<float> _normals;
    std::vector<float> _texcoords;
    std::vector<GLuint> _indices;
    std::string _modelname;

    //InterInfo ** _rayTraceResult;
    //vector<vector<InterInfo>> _rayTraceResult;
    //vector<int> _hitRayNumber;

    // Model rotation.
    float _theta;
    float _rx, _ry, _rz;
};

#endif
