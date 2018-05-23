#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <ctime>
#include <cmath>
#include <vector>
#include <iostream>

#include "glm\glm.hpp"

#include "toolFunction.h"
#include "SphericalHarmonic.h"


using glm::vec3;
using glm::vec2;
using std::vector;


struct Sample
{
	vec3 _cartesCoord;
	vec2 _sphericalCoord;//theta,phi
	float* _SHvalue;
	Sample(vec3 car_in,vec2 sph_in):_cartesCoord(car_in),_sphericalCoord(sph_in){}
};

class Sampler
{
public:
	Sampler(unsigned n); // sqrt of sample number
	void computeSH(int band);//band means l ,quite important

	vector<Sample> _samples;

};



#endif