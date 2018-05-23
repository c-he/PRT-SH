#ifndef DIFFUSEOBJECT_H_
#define DIFFUSEOBJECT_H_


#include "Object.h"
#include "toolFunction.h"


class DiffuseObject : public Object
{

public:
	void project2SH(int mode,int band,int sampleNumber,int bounce);

	void write2Disk(string filename);
	void write2Diskbin(string filename);
	void readFDisk(string filename);
	void readFDiskbin(string filename);

	vector<vector<vec3>> _DTransferFunc[3];

private:
	
	void diffuseUnshadow(int size,int band2,Sampler *sampler,int type,BVHTree* Inbvht);
	void diffuseShadow(int size,int band2,Sampler *sampler, int type,BVHTree* Inbvht);
	void interreflect(int size,int band2,Sampler *sampler,int type,int bounce);


	vector<vector<vec3>> _TransferFunc;//transform function for diffuse material
	

};




#endif