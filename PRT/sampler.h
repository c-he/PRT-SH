#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <vector>
#include <glm/glm.hpp>

#include "utils.h"
#include "sphericalHarmonics.h"

using glm::vec3;
using glm::vec2;
using std::vector;

class Sample
{
public:
    glm::vec3 _cartesCoord;
    glm::vec2 _sphericalCoord; // theta, phi
    float* _SHvalue;

    Sample(glm::vec3 car_in, glm::vec2 sph_in): _cartesCoord(car_in), _sphericalCoord(sph_in)
    {
    }
};

class Sampler
{
public:
    // sqrt of sample number.
    Sampler(unsigned int n);
    // band means l.
    void computeSH(int band);

    std::vector<Sample> _samples;
};


#endif
