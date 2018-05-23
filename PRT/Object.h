#ifndef OBJECT_H_
#define OBJECT_H_

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glm\glm.hpp"


#include "Sampler.h"
#include "BVHTree.h"
#include "toolFunction.h"

using std::stringstream;
using std::ifstream;
using std::string;
using std::vector;

const int PREDIFFUSE = 1;
const int PRESHADOW = 1 << 1;
const int PREINTERREFLECT = 1 << 2;

struct InterInfo
{
	bool _interornot;
	Ray _ray;
	InterInfo()
	{
		_interornot = false;
	}
	InterInfo(bool in ,Ray rayin)
		:_interornot(in),_ray(rayin)
	{}

};

class Object
{
public:
	Object(){_theta = 0.0f;_rx = _ry = 0.0f;_rz = 1.0f;_difforGeneral = false;/*_rayTraceResult = NULL;*/}
	void init(string path,vec3 albedo);
	//project to SH function
	virtual void project2SH(int mode,int band,int sampleNumber,int bounce){};
	virtual void write2Disk(string filename){};
	virtual void readFDisk(string filename){};

	bool intersectTest(Ray &ray,int facenumber);
	
	void normVertexes();
	void setRotate(float theta,float x,float y,float z)
	{
		_theta = theta;
		_rx = x;_ry = y;_rz = z;
	}
	int band(){return _band;}
	


protected:
	float _vmaxX,_vmaxY,_vmaxZ;
	float _vminX,_vminY,_vminZ;

	vec3 _albedo;
	int _band;

	bool _difforGeneral;//false means diffuse


public:
	vector<float> _vertexes;
	vector<float> _normals;
	vector<float> _texcoords;

	vector<unsigned> _renderIndex;

	string _modelname;


	//InterInfo ** _rayTraceResult;
	//vector<vector<InterInfo>> _rayTraceResult;
	//vector<int> _hitRayNumber;


//	//model rotate
	float _theta;
	float _rx,_ry,_rz;

};





#endif