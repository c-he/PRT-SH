#ifndef SIMPLELIGHTING_H_
#define SIMPLELIGHTING_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include "glm/glm.hpp"
using glm::vec3;

inline	vec3 Simplelight(float theta, float phi)
{
	//if((theta > (M_PI/4.0f)) && (theta < (0.75f * M_PI)) && (fabs(phi) <= M_PI/8.0f))
	//if(fabs(phi - M_PI/2.0f) <= M_PI/4.0f)

	if(fabs(theta) <= M_PI/8.0f)
		return vec3(8.0f,8.0f,8.0f);
	else 
		return vec3(0.0f,0.0f,0.0f);
}
	


#endif