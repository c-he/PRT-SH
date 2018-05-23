#ifndef LIGHT_H_
#define LIGHT_H_

#include <string>
#include <fstream>
#include <iostream>


#include "CImg/CImg.h"

#include "Eigen\Dense"
#include "glm\glm.hpp"


#include "Sampler.h"
#include "HDRLoader.h"
#include "SHrotation.h"
#include "toolFunction.h"
#include "SimpleLighting.h"

using std::string;
using glm::mat4;
using Eigen::VectorXf;
using Eigen::MatrixXf;


enum LightType{PROBE,CUBEMAP};
class Lighting
{
public:
	Lighting(){}
	Lighting(string path,LightType type,int band);//construction function for process
	
	Lighting(int band,VectorXf coeffs[3])
	{
		_band = band;
		int band2  = band * band;

		for(int k = 0;k <3; ++k)
		{
			_Vcoeffs[k].resize(band2);
			for(int i = 0;i < band2;++i)
				_Vcoeffs[k](i) = coeffs[k](i);
		}

		for(int i = 0;i < band2; ++i)
			_coeffs.push_back(vec3(coeffs[0](i),coeffs[1](i),coeffs[2](i)));

	}
	
	void init(string CoeffPath,vec3 HDRaffect,vec3 Glossyaffect);

	vec3 probeColor(vec3 dir);
	void process(int sampleNumber, bool image);
	void write2Disk(string outFile);

	void rotateZYZ(vector<vec2> &para);
	vec3 HDRaffect(){return _HDRaffect;}
	vec3 Glossyaffect(){return _Glossyaffect;}


	int band(){return _band;}

	vector<vec3> _coeffs;
	VectorXf  _Vcoeffs[3];

private:
	LightType _ltype;
	string _path;

	vec3 _HDRaffect;
	vec3 _Glossyaffect;
	HDRLoaderResult _HDRresult;


	vec3 *_pixels;
	int _width;
	int _height;

	int _band;// means the band of SH function
};



#endif