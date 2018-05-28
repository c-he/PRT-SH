#ifndef BVHTREE_H_
#define BVHTREE_H_

#include <vector>
using std::vector;

#include "boundingBox.h"
#include "object.h"

const int XAXIS = 0;
const int YAXIS = 1;
const int ZAXIS = 2;

class Object;


struct BVHNode
{
	bool _leafornot;

	BVHNode *_left,*_right;

	Triangle *_tri0,*_tri1;

	BBox _bbox;


	void setNULL()
	{
		_leafornot = false;// interior node
		_left = _right = NULL;
		_tri0 = _tri1 = NULL;

	}

	//BVHNode()
	BVHNode()
	{
		setNULL();
	}

	BVHNode(Triangle &t1)
	{
		setNULL();
		_bbox = BBox(t1);
		_tri0 = new Triangle(t1);
		_leafornot = true;
	}
	BVHNode(Triangle &t1,Triangle &t2)
	{
		setNULL();
		_bbox = merge(BBox(t1),BBox(t2));
		_tri0 = new Triangle(t1);
		_tri1 = new Triangle(t2);
		_leafornot = true;
	}

	bool hit(Ray &ray)
	{

		if(!_bbox.rayIntersect(ray))	
		{
			return false;
		}
		else
		{
			if(_leafornot == true)
			{

				if(rayTriangle(ray,*_tri0))
				{
					return true;
				}

				if(_tri1 == NULL)
				{
					return false;
				}

				return (rayTriangle(ray,*_tri1));
			}

			return ((_left->hit(ray))||(_right->hit(ray)));
		}

	}
};

class BVHTree
{
public:
    BVHTree() = default;
	void build( Object &obj);
	BVHNode * buildBranch(int start,int size, int axis,int &faceNumber);
	int split(int start,int size, float pivot, int axis);
	bool interTest(Ray &ray);

private:
	vector<Triangle> _triangles;
	BVHNode *_root;

	vector<Triangle> _nodeTri;
};

#endif
