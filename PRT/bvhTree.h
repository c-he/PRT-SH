#ifndef BVHTREE_H_
#define BVHTREE_H_

#include <vector>

#include "boundingBox.h"
#include "object.h"

#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

struct BVHNode
{
    bool _leaf;
    BVHNode *_left, *_right;
    Triangle *_tri0, *_tri1;
    BBox _bbox;

    //BVHNode()
    BVHNode()
    {
        setNULL();
    }

    BVHNode(Triangle& t1)
    {
        setNULL();
        _bbox = BBox(t1);
        _tri0 = new Triangle(t1);
        _leaf = true;
    }

    BVHNode(Triangle& t1, Triangle& t2)
    {
        setNULL();
        _bbox = merge(BBox(t1), BBox(t2));
        _tri0 = new Triangle(t1);
        _tri1 = new Triangle(t2);
        _leaf = true;
    }

    bool hit(Ray& ray)
    {
        if (!_bbox.rayIntersect(ray))
        {
            return false;
        }
        if (_leaf)
        {
            if (rayTriangle(ray, *_tri0))
            {
                return true;
            }

            if (_tri1 == nullptr)
            {
                return false;
            }

            return (rayTriangle(ray, *_tri1));
        }

        return ((_left->hit(ray)) || (_right->hit(ray)));
    }

    void setNULL()
    {
        // Interior node.
        _leaf = false;
        _left = _right = nullptr;
        _tri0 = _tri1 = nullptr;
    }
};

class BVHTree
{
public:
    BVHTree() = default;
    void build(Object& obj);
    bool intersect(Ray& ray);

private:
    int split(int start, int size, float pivot, int axis);
    BVHNode* recursiveBuild(int start, int size, int axis, int& faceNumber);

    std::vector<Triangle> _triangles;
    std::vector<Triangle> _nodeTri;
    BVHNode* _root;
};

#endif
