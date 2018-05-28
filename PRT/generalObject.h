#ifndef GENERALOBJECT_H_
#define GENERALOBJECT_H_


#include <vector>
#include <fstream>
#include "Eigen\Dense"

#include "glm/glm.hpp"

#include "Object.h"
#include "Sampler.h"

using glm::vec3;
using glm::vec4;
using std::vector;
using Eigen::MatrixXf;
using Eigen::VectorXf;

class GeneralObject: public Object
{
	
public:
	GeneralObject(){_difforGeneral = true;_glossiness = 4.0f;}
	void project2SH(int mode,int band,int sampleNumber,int bounce);
	void write2Disk(string filename);
	void readFDisk(string filename);

	void computeTBN();
	void computeKernel();
	void setGlossiness();




private:

	void unshadowed(int size, int band2,Sampler *sampler,int type);
	void interReflect(int size, int band2,Sampler *sampler,int type,int bounce);
public:
	vector<MatrixXf> _TransferMatrix[2];

	//vector<vec3> _tan1;
	//vector<vec3> _tan2;
	vector<vec4> _tangent;

	VectorXf _glossyKernel[3];

	VectorXf _BRDFcoeff;

	float _glossiness;

};




#endif