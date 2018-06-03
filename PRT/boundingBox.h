#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include <vector>
#include <algorithm>
#include <glm/glm.hpp>

#include "utils.h"

class BBox
{
public:
    BBox() = default;
    BBox(glm::vec3 pMin, glm::vec3 pMax);
    BBox(Triangle& in);
    BBox(std::vector<Triangle>& inlist);

    bool rayIntersect(Ray& ray);

    float area()
    {
        glm::vec3 diff = _v[1] - _v[0];
        return 2.0f * (diff.x * diff.y + diff.x * diff.z + diff.y * diff.z);
    }

    float volume()
    {
        glm::vec3 diff = _v[1] - _v[0];
        return diff.x * diff.y * diff.z;
    }

    //_v[0] for min,_v[1] for max
    glm::vec3 _v[2];
    glm::vec3 _center;

private:
    void setCenter()
    {
        _center = (_v[0] + _v[1]) / 2.0f;
    }
};

inline BBox merge(BBox b1, BBox b2)
{
    glm::vec3 pMin, pMax;
    pMin.x = std::min(b1._v[0].x, b2._v[0].x);
    pMin.y = std::min(b1._v[0].y, b2._v[0].y);
    pMin.z = std::min(b1._v[0].z, b2._v[0].z);

    pMax.x = std::max(b1._v[1].x, b2._v[1].x);
    pMax.y = std::max(b1._v[1].y, b2._v[1].y);
    pMax.z = std::max(b1._v[1].z, b2._v[1].z);

    return BBox(pMin, pMax);
};

#endif
