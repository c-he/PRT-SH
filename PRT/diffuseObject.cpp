#include "DiffuseObject.h"


void DiffuseObject::write2Disk(string filename)
{
	std::ofstream out(filename);
	int size = _vertexes.size()/3;
	int band2 = _band * _band;

	out << _vertexes.size() << std::endl;
	out << _band << std::endl;

	for(int i = 0;i < size; ++i)
	{
		for(int j = 0;j < band2; ++j)
		{
			/*if(_isnan(_TransferFunc[i][j].x)
				|| _isnan(_TransferFunc[i][j].y)
				|| _isnan(_TransferFunc[i][j].z)
				)
			{
				std::cout << "NaN!"<< std::endl;
				exit(-1);
			}*/

			out << _TransferFunc[i][j].x  << ' '
				<< _TransferFunc[i][j].y  << ' '
				<< _TransferFunc[i][j].z  << ' ';
				
		}
		out << std::endl;

	}

	out.close();
	std::cout << "Write done." << std::endl;

}

void DiffuseObject::write2Diskbin(string filename)
{
	std::ofstream out;
	out.open(filename.c_str(),std::ofstream::binary);
	int size = _vertexes.size()/3;
	int band2 = _band * _band;

	out.write((char *)&size,sizeof(unsigned int));
	out.write((char *)&_band,sizeof(int));
	//out << _band << std::endl;

	for(int i = 0;i < size; ++i)
	{
		for(int j = 0;j < band2; ++j)
		{
			/*out << _TransferFunc[i][j].x  << ' '
				<< _TransferFunc[i][j].y  << ' '
				<< _TransferFunc[i][j].z  << ' ';*/
			out.write((char *)&_TransferFunc[i][j].x,sizeof(float));
			out.write((char *)&_TransferFunc[i][j].y,sizeof(float));
			out.write((char *)&_TransferFunc[i][j].z,sizeof(float));

				
		}
		//out << std::endl;

	}

	out.close();
	std::cout << "Write done." << std::endl;

}
	
void DiffuseObject::readFDisk(string filename)
{
	string transf[3] = {"D01.txt","DS01.txt","DSI01.txt"};

	for(int i = 0;i < 3; ++i)
		_DTransferFunc[i].clear();

	for(int k = 0;k < 3; ++k)
	{
		string unshadowed = filename + transf[k];
		std::ifstream in(unshadowed);
		int size, band2;

		in >> size >> _band;

		band2 = _band * _band;

		vector<vec3> empty(band2,vec3(0,0,0));

		for(int i = 0; i < size; ++i)
		{
			_DTransferFunc[k].push_back(empty);

			for(int j = 0;j < band2; ++j)
			{
				in >> _DTransferFunc[k][i][j].x 
				   >> _DTransferFunc[k][i][j].y
				   >> _DTransferFunc[k][i][j].z ;
			}
		}

		in.close();
	}

}

void DiffuseObject::readFDiskbin(string filename)
{
	// string transf[3] = {"D01.dat","DS01.dat","DSI01.dat"};
	string transf[2] = {"D01.dat","DS01.dat"};

	for(int i = 0;i < 2; ++i)
		_DTransferFunc[i].clear();

	for(int k = 0;k < 2; ++k)
	{
		string unshadowed = filename + transf[k];
		std::ifstream in(unshadowed,ifstream::binary);
		unsigned int size, band2;
		
		in.read((char *)&size,sizeof(unsigned int));
		in.read((char *)&_band,sizeof(int));

		//in >> size >> _band;


		band2 = _band * _band;

		vector<vec3> empty(band2,vec3(0,0,0));

		for(unsigned i = 0; i < size; ++i)
		{
			_DTransferFunc[k].push_back(empty);

			for(unsigned j = 0;j < band2; ++j)
			{
			/*	in >> _DTransferFunc[k][i][j].x 
				   >> _DTransferFunc[k][i][j].y
				   >> _DTransferFunc[k][i][j].z ;*/

				in.read((char *)&_DTransferFunc[k][i][j].x,sizeof(float));
				in.read((char *)&_DTransferFunc[k][i][j].y,sizeof(float));
				in.read((char *)&_DTransferFunc[k][i][j].z,sizeof(float));
			}
		}

		in.close();
	}


}

void DiffuseObject::diffuseUnshadow(int size,int band2,Sampler *sampler,int type,BVHTree* Inbvht = NULL)
{
	bool shadow = false;
	if(type > 1)	shadow = true;

	bool visiblity = true;
	int sampNumber = sampler->_samples.size();

	BVHTree bvht;

	vector<vec3> empty(band2,vec3(0.0f,0.0f,0.0f));
	_TransferFunc.resize(size,empty);

	if(shadow)
	{
		if(type < 7)
			bvht.build(*this);
		else
			bvht = *Inbvht;
	}

#pragma omp parallel for
	for(int i = 0;i < size;++i)
	{
		int index = 3 * i;

		vec3 normal = vec3(
			_normals[index],
			_normals[index + 1],
			_normals[index + 2]
		);

		int negeNumber = 0;
		int vNumber = 0;
		for(int j = 0;j < sampNumber; ++j)
		{
			Sample stemp = sampler->_samples[j];
			float geo = glm::dot(normal,stemp._cartesCoord);

			if(geo <= 0.0f)
			{
				negeNumber++;
				continue;
			}
			//shadow
			if(shadow)
			{
				Ray testRay(vec3(
					_vertexes[index],
					_vertexes[index + 1],
					_vertexes[index + 2]),
					stemp._cartesCoord
					);
				testRay._o += 2*M_DELTA * normal;
				visiblity = !bvht.interTest(testRay);
				
				if(visiblity == true)
				{
					vNumber++;
				}

			}
			else
			{
				visiblity = true;
			}

			if(!visiblity)
				continue;

			assert(fabs(geo) <= 1.0f);

			for(int k = 0;k < band2; ++k)
			{
				float SHvalue = stemp._SHvalue[k];

				_TransferFunc[i][k] += _albedo * SHvalue * geo;
			}
		}

		//std::cout <<"posNumber:" <<  (4096 - negeNumber)<< endl;
		//std::cout <<"vNumber:" <<  vNumber << endl;

		//_hitRayNumber[i] = sampNumber - vNumber;

//		if(!(i%10))
	//		std::cout << i << std::endl;
	}

	float weight = 4.0f * (float)MY_PI/sampler->_samples.size();
#pragma omp parallel for
	for(int i = 0;i < size; ++i)
	{
		//std::cout << i << std::endl;
		for(int j = 0;j < band2; ++j)
		{
			_TransferFunc[i][j] *= weight;
		}
	}
	if(type == 1)
		std::cout<< "Unshadowed Transfer function done!" << std::endl;

}

void DiffuseObject::diffuseShadow(int size, int band2,Sampler *sampler,int type,BVHTree* Inbvht = NULL)
{
	std::cout <<"Shadow begin" << std::endl;

	diffuseUnshadow(size,band2,sampler,type,Inbvht);
	if(type == 3)
		std::cout << "Shadowed Transfer function done!" << std::endl;
	//system("pause");
}

void DiffuseObject::interreflect(int size,int band2,Sampler *sampler,int type,int bounce = 1)
{

	//ifstream in(_modelname + "rtresult.txt");

	BVHTree bvht;
	bvht.build(*this);

	diffuseShadow(size,band2,sampler,type,&bvht);


	int sampleNumber = sampler->_samples.size();



	vector< vector<vec3> > *interReflect = new vector< vector<vec3>>[bounce + 1];

	interReflect[0] = _TransferFunc;
	vector<vec3> empty(band2,vec3(0.0f,0.0f,0.0f));

	float weight = 4.0f *(float)MY_PI/sampleNumber;

	for(int k = 0;k < bounce; ++k)
	{
		vector< vector<vec3 >> zeroVector(size, empty);
		interReflect[k + 1].resize(size);

#pragma omp parallel for
		for(int i = 0;i < size; ++i)
		{
			int offset = 3 * i;
			vec3 normal = vec3(_normals[offset],_normals[offset + 1],_normals[offset + 2]);
			
			//int indirectSize = _rayTraceResult[i].size();

			for(int j = 0;j < sampleNumber; ++j)
			{
				Sample stemp = sampler->_samples[j];
					Ray rtemp(vec3(
					_vertexes[offset],
					_vertexes[offset + 1],
					_vertexes[offset + 2]),
					stemp._cartesCoord
					);
				rtemp._o += 2*M_DELTA * normal;

				bool visiblity = !bvht.interTest(rtemp);
				if(visiblity)
					continue;
				//Ray rtemp;

			/*	in >> rtemp._dir.x >> rtemp._dir.y >> rtemp._dir.z >> rtemp._index 
					>> rtemp._o.x >> rtemp._o.y >> rtemp._o.z >> rtemp._t >> rtemp._tmax >> rtemp._tmin;*/

				//rtemp.normalize();

				float geo = glm::dot(rtemp._dir,normal);
				if(geo <= 0)	continue;
				
				int triIndex = 3 * rtemp._index;
				int voffset[3];
				vec3 p[3];
				vector<vec3> *SHTrans[3];
				for(int m = 0;m < 3; ++m)
				{
					voffset[m] = _renderIndex[triIndex + m];
					SHTrans[m] = &interReflect[k][voffset[m]];
					voffset[m] *= 3;
					p[m] = vec3(_vertexes[voffset[m]],_vertexes[voffset[m] + 1],_vertexes[voffset[m] + 2]);

				}
				vec3 pc = rtemp._o + rtemp._t * rtemp._dir;

				float u,v,w;
				barycentric(pc,p,u,v,w);//barycentric for interpolate

				vector<vec3> SHtemp;
				SHtemp.resize(band2);

				for(int m = 0; m < band2; ++m)
				{
					SHtemp[m] = w * SHTrans[0]->at(m) + u * SHTrans[1]->at(m) + v * SHTrans[2]->at(m);
					zeroVector[i][m]  += geo * _albedo * SHtemp[m];
				}
			}
		}

#pragma omp parallel for
		for(int i = 0;i < size; ++i)
		{
			interReflect[k + 1][i].resize(band2);
			for(int j = 0;j < band2; ++j)
			{
				zeroVector[i][j] *= weight;
				interReflect[k + 1][i][j] = interReflect[k][i][j] + zeroVector[i][j];
			}
		}

	}
	_TransferFunc = interReflect[bounce];
	std::cout<< "InterReflect Transfer function done!" << std::endl;
	//in.close();
}

void DiffuseObject::project2SH(int mode,int band,int sampleNumber,int bounce = 1)
{
	_band = band;

	int size = _vertexes.size()/3;
	int band2 = band * band;

	Sampler stemp((unsigned)sqrt(sampleNumber));
	stemp.computeSH(band);


	if(mode == 1)
	{
		diffuseUnshadow(size,band2,&stemp,1);
	}
	else if(mode == 3)
	{
		diffuseShadow(size,band2,&stemp,3);
	}
	else if(mode == 7)
	{
		interreflect(size,band2,&stemp,7,bounce);
	}

}
