#ifndef BRDF_H_
#define BRDF_H_

#include <Eigen/Dense>

enum BRDF_TYPE
{
    Phong,
    AS,
    PF,
    vinyl,
    alum
};

class BRDF
{
public:
    BRDF()
    {
        sampleNumber = 64;
    }

    void init(int band, BRDF_TYPE type);

    int sampleNumber;
    Eigen::VectorXf** _BRDFlookupTable;
};

#endif
