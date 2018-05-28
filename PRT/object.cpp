#include "Object.h"

void Object::normVertexes()
{
	int size = _vertexes.size();
	float scaleX = std::max(fabs(_vmaxX),fabs(_vminX));
	float scaleY = std::max(fabs(_vmaxY),fabs(_vminY));
	float scaleZ = std::max(fabs(_vmaxZ),fabs(_vminZ));

	float weight = 1.0f/Trimax(scaleX,scaleY,scaleZ);

	for(int i = 0;i < size;++i)
	{
		_vertexes[i] *= weight;
	}
}

void Object::init(string path,vec3 albedo)
{	
	_modelname = path;
	_albedo = albedo;
	bool b_texture = (path == "Scene/Cow.obj" || path == "Scene/buddha.obj" || path == "Scene/dragon.obj" || path == "Scene/Cup.obj" || path == "Scene/Cube.obj"||path == "Scene/Gargoyle2.obj" ||path == "Scene/Sphere.obj" ||path == "Scene/manneq.obj"||path == "Scene/maxplanck.obj")?true:false;
	std::ifstream in;
	try
	{
		in.open(path);
	}
	catch(...)
	{
		printf("Scene loaded error");
	}
	if(!in.is_open())
	{
		std::cout << "Obj not opened!" << std::endl;
		exit(0);
	}
	string line,attribute;

	float x,y,z,u,v,nx,ny,nz;
	char mask;
	unsigned index_v0,index_v1,index_v2;
	unsigned index_n0,index_n1,index_n2;
	unsigned index_t0,index_t1,index_t2;

	_vmaxX = _vmaxY = _vmaxZ = -(float)M_INFINITE;
	_vminX = _vminY = _vminZ = (float)M_INFINITE;

	while(getline(in,line))
	{
		std::istringstream s_line(line);
		s_line >> attribute;
		if(attribute == "#")
			continue;
		else if(attribute == "v")
		{
			s_line >> x >> y >> z;

			if(x > _vmaxX)_vmaxX = x;
			if(x < _vminX)_vminX = x;
			if(y > _vmaxY)_vmaxY = y;
			if(y < _vminY)_vminY = y;
			if(z > _vmaxZ)_vmaxZ = z;
			if(z < _vminZ)_vminZ = z;

			_vertexes.push_back(x);
			_vertexes.push_back(y);
			_vertexes.push_back(z);
		}
		else if(attribute == "vt")
		{
			s_line >> u >> v;
			_texcoords.push_back(u);
			_texcoords.push_back(v);
		}
		else if(attribute == "vn")
		{
			s_line >> nx >> ny >> nz;
			_normals.push_back(nx);
			_normals.push_back(ny);
			_normals.push_back(nz);
		}
		else if(attribute == "f")
		{
			if(b_texture)
			{
				s_line >> index_v0 >> mask >> index_n0 >> mask >> index_t0
					   >> index_v1 >> mask >> index_n1 >> mask >> index_t1
					   >> index_v2 >> mask >> index_n2 >> mask >> index_t2;
			}
			else
			{
				s_line >> index_v0 >> mask >> mask >> index_n0
					   >> index_v1 >> mask >> mask >> index_n1 
					   >> index_v2 >> mask >> mask >> index_n2;

			}

			_renderIndex.push_back(index_v0 - 1);
			_renderIndex.push_back(index_v1 - 1);
			_renderIndex.push_back(index_v2 - 1);
		}

	}
	in.close();

	normVertexes();

	//colinear debug
	/*for(int i = 0;i < _vertexes.size()/3; ++i)
	{
		int index = 3 *i ;
		int offset[3];
		vec3 p[3];
		vec3 pc = vec3(0.0f,0.0f,0.0f);
		float u,v,w;
		for(int j = 0;j < 3; ++j)
		{
			offset[j] = 3 * _renderIndex[index + j];
			p[j] = vec3(_vertexes[offset[j]],_vertexes[offset[j] + 1],_vertexes[offset[j] + 2]);
		}

		barycentric(pc,p,u,v,w);
	}*/
}


bool Object::intersectTest(Ray &ray,int facenumber)
{
	bool result = false;
	
	//naive approach O(n)
	for(int i = 0;i < facenumber;++i)
	{
		int offset = 3 * i;
		int index[3];
		index[0] = _renderIndex[offset];
		index[1] = _renderIndex[offset + 1];
		index[2] = _renderIndex[offset + 2];

		vec3 p[3];
		for(int j = 0;j < 3; ++j)
		{
			int Vindex = 3 * index[j];
			p[j] = vec3(_vertexes[Vindex],_vertexes[Vindex + 1],_vertexes[Vindex + 2]);
		}
		//DEBUG change the intersect

		Triangle Ttemp(p[0],p[1],p[2],i);

		if(rayTriangle(ray,Ttemp))
		{
			result = true;
			break;
		}
		
	}

	return result;
}


