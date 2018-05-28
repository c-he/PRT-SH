#ifndef LIGHT_H_
#define LIGHT_H_

#include <string>
#include <Eigen/Dense>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "sampler.h"
#include "shRotation.h"
#include "utils.h"
#include "simpleLighting.h"

using std::string;
using glm::mat4;
using Eigen::VectorXf;
using Eigen::MatrixXf;

enum LightType { PROBE, CUBEMAP };

class Lighting
{
public:
    Lighting() = default;
    Lighting(string path, LightType type, int band); //construction function for process

    Lighting(int band, VectorXf coeffs[3])
    {
        _band = band;
        int band2 = band * band;

        for (int k = 0; k < 3; ++k)
        {
            _Vcoeffs[k].resize(band2);
            for (int i = 0; i < band2; ++i)
                _Vcoeffs[k](i) = coeffs[k](i);
        }

        for (int i = 0; i < band2; ++i)
            _coeffs.push_back(vec3(coeffs[0](i), coeffs[1](i), coeffs[2](i)));
    }

    ~Lighting();

    void init(string CoeffPath, vec3 HDRaffect, vec3 Glossyaffect);

    vec3 probeColor(vec3 dir);
    void process(int sampleNumber, bool image);
    void write2Disk(string outFile);

    void rotateZYZ(vector<vec2>& para);
    vec3 HDRaffect() { return _HDRaffect; }
    vec3 Glossyaffect() { return _Glossyaffect; }


    int band() { return _band; }

    vector<vec3> _coeffs;
    VectorXf _Vcoeffs[3];

private:
    LightType _ltype;
    string _path;

    vec3 _HDRaffect;
    vec3 _Glossyaffect;

    vec3* _pixels;
    int _width;
    int _height;
    float* _data;
    GLuint _Format;
    GLuint _Type;
    GLuint _InternalFormat;

    int _band; // means the band of SH function
};

#endif
