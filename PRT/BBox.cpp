#include "BBox.h"


inline BBox::BBox(Triangle &in)
{
	_v[0].x = Trimin(in._v0.x,in._v1.x,in._v2.x);
	_v[0].y = Trimin(in._v0.y,in._v1.y,in._v2.y);
	_v[0].z = Trimin(in._v0.z,in._v1.z,in._v2.z);
	_v[1].x = Trimax(in._v0.x,in._v1.x,in._v2.x);
	_v[1].y = Trimax(in._v0.y,in._v1.y,in._v2.y);
	_v[1].z = Trimax(in._v0.z,in._v1.z,in._v2.z);

	this->setCenter();
}

BBox::BBox(vector<Triangle> &inlist)
{
	int size = inlist.size();

	if(size == 0)
	{
		std::cout << "BBox list init error" << std::endl;
	}
	_v[0] = inlist[0]._v0;
	_v[1] = inlist[1]._v0;

	for(int i = 0;i < size; ++i)
	{
		BBox btemp(inlist[i]);

		for(int j = 0;j < 3; ++j)
		{
			if(btemp._v[0][j] < _v[0][j])
			{
				_v[0][j] = btemp._v[0][j];
			}
			if(btemp._v[1][j] > _v[1][j])
			{
				_v[1][j] = btemp._v[1][j];
			}

		}
	}
	this->setCenter();

}

// the ray should be normalized first
bool BBox::rayIntersect(Ray &r)
{

	float tmin = 0;
	float tmax = (float)M_INFINITE;

	for(int i = 0;i < 3; ++i)
	{
		int porN = (r._dir[i] >= 0 )? 0:1;

		float t0 = (_v[porN][i] - r._o[i]) * r._inv[i];
		float t1 = (_v[1 - porN][i] - r._o[i]) * r._inv[i];

		if(t0 > tmin) tmin = t0;
		if(t1 < tmax) tmax = t1;
		if(tmin > tmax) return false;
	}

	return true;
}
	