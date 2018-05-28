#include "bvhTree.h"

int BVHTree::split(int start, int size, float pivot, int axis)
{
	BBox btemp;
	float centroid;
	int index = 0;

	for(int i = start;i < (start + size); ++i)
	{
		btemp = BBox(_triangles[ i]);
		centroid = btemp._center[axis];

		//assert(btemp._center == (btemp._v[0] + btemp._v[1])/2.0f);

		if(centroid < pivot)
		{
			Triangle temp = _triangles[i];
			_triangles[i] = _triangles[start + index];
			_triangles[start + index] = temp;
			index++;
		}
	}

	if((index == 0) || (index == size)) index = size/2;
	return index;
}

bool BVHTree::interTest(Ray &ray)
{
	ray.normalize();
	bool result = _root->hit(ray);
	return result;
}

void BVHTree::build(Object &obj)
{
	std::cout <<"BVHTree Build." << std::endl;
	int faceNumber = 0;

	int facenumber = obj._renderIndex.size()/3;
	_triangles.clear();

	for(int i = 0;i < facenumber; ++i)
	{
		int offset = 3 * i;

		Triangle triangle;
		vec3 v[3];
		int index[3];

		for(int j = 0;j < 3; ++j)
		{
			index[j] = 3 * obj._renderIndex[offset + j];

			v[j].x = obj._vertexes[index[j]];
			v[j].y = obj._vertexes[index[j] + 1];
			v[j].z = obj._vertexes[index[j] + 2];
		}
		//debug

	/*	vec3 pc(0.0f,0.0f,0.0f);
		float u = 0.0f;
		float x = 0.0f;
		float w = 0.0f;
		barycentric(pc,v,u,x,w);*/


		//debug

		triangle._v0 = v[0];
		triangle._v1 = v[1];
		triangle._v2 = v[2];

		triangle._index = i;

		_triangles.push_back(triangle);
	}

	_root = new BVHNode();

	
	if(_triangles.size() == 1)
	{
		_root = new BVHNode(_triangles[0]);
	}
	if(_triangles.size() == 2)
	{

		_root = new BVHNode(_triangles[0],_triangles[1]);
	}

	_root->_bbox = BBox(_triangles);

	vec3 pivot = _root->_bbox._center;

	int mid_point = split(0,_triangles.size(),pivot[0],XAXIS);

	_root->_left = buildBranch(0,mid_point,YAXIS,faceNumber);
	_root->_right = buildBranch(mid_point,_triangles.size() - mid_point,YAXIS,faceNumber);
	std::cout <<"BVHTree Build Done." << std::endl;
}

BVHNode *BVHTree::buildBranch(int start,int size, int axis,int &faceNumber)
{
	if(size < 1)
	{
		std::cout << "Size Error!" << std::endl;
	}
	if(size == 1)
	{
		return new BVHNode(_triangles[start]);
	}
	if(size == 2)
	{
		return new BVHNode(_triangles[start],_triangles[start + 1]);
	}

	BBox btemp = BBox(_triangles[start]);

	for(int i = start+1; i < start + size; ++i)
		btemp = merge(btemp,BBox(_triangles[i]));

	vec3 pivot = btemp._center;

	int mid_point = split(start,size,pivot[axis],axis);

	BVHNode *result = new BVHNode();

	result->_bbox = btemp;
	result->_left = buildBranch(start,mid_point,((axis + 1)% 3),faceNumber);
	result->_right = buildBranch(start + mid_point,size - mid_point,(axis + 1)%3,faceNumber);

	return result;
}


