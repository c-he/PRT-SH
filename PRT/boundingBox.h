#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include <vector>
#include <algorithm>
#include <glm/glm.hpp>

#include "utils.h"

using glm::vec3;
using std::vector;

class BBox
{
public:
	BBox() = default;
	BBox(Triangle &in);
	BBox(vector<Triangle> &inlist);
	

	bool rayIntersect(Ray &ray);

	void setCenter()
	{
		_center = (_v[0] + _v[1])/2.0f;
	}

	float area()
	{
		vec3 diff = _v[1] - _v[0];
		return 2.0f *(diff.x * diff.y + diff.x * diff.z + diff.y * diff.z);
	}

	float volumn()
	{
		vec3 diff = _v[1] - _v[0];
		return diff.x * diff.y * diff.z;
	}


	vec3 _v[2];//_v[0] for min,_v[1] for max
	vec3 _center;
};

inline BBox merge(BBox b1, BBox b2)
{
	BBox result;
	result._v[0].x = std::min(b1._v[0].x, b2._v[0].x);
	result._v[0].y = std::min(b1._v[0].y, b2._v[0].y);
	result._v[0].z = std::min(b1._v[0].z, b2._v[0].z);

	result._v[1].x = std::max(b1._v[1].x,b2._v[1].x);
	result._v[1].y = std::max(b1._v[1].y,b2._v[1].y);
	result._v[1].z = std::max(b1._v[1].z,b2._v[1].z);

	result.setCenter();

	return result;
};

#endif
