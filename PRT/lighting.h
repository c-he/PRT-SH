#ifndef LIGHT_H_
#define LIGHT_H_

#include <string>
#include <vector>
#include <Eigen/Dense>
#include <glm/glm.hpp>

enum LightType { PROBE, CROSS };

class Lighting
{
public:
    Lighting() = default;
    // Constructor for preprocessing.
    Lighting(std::string path, LightType type, int band);
    Lighting(int band, Eigen::VectorXf coeffs[3]);

    ~Lighting();

    void init(std::string coeffPath, glm::vec3 hdrEffect, glm::vec3 glossyEffect);
    glm::vec3 probeColor(glm::vec3 dir);
    void process(int sampleNumber, bool image = true);
    void write2Diskbin(std::string outFile);
    void rotateZYZ(std::vector<glm::vec2>& para);

    glm::vec3 hdrEffect() { return _hdrEffect; }
    glm::vec3 glossyEffect() { return _glossyEffect; }
    int band() { return _band; }

    std::vector<glm::vec3> _coeffs;
    Eigen::VectorXf _Vcoeffs[3];

private:
    LightType _ltype;
    std::string _path;

    glm::vec3 _hdrEffect;
    glm::vec3 _glossyEffect;

    int _width;
    int _height;
    // The band of SH basis function.
    int _band;
    float* _data;
};

#endif
