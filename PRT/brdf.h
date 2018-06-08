#ifndef BRDF_H_
#define BRDF_H_

#include <Eigen/Dense>

#define SAMPLE_NUMBER 64
#define SHOW_BRDF

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
        sampleNumber = SAMPLE_NUMBER;
    }

    void init(int band, BRDF_TYPE type);

    int sampleNumber;
    Eigen::VectorXf** _BRDFlookupTable;
};

#endif
