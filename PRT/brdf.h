#ifndef BRDF_H_
#define BRDF_H_

#include <Eigen/Dense>

#define SAMPLE_NUMBER 64
// #define SHOW_BRDF

enum BRDF_TYPE
{
    BRDF_PHONG,
    BRDF_AS,
    BRDF_PF,
    BRDF_vinyl,
    BRDF_alum
};

class BRDF
{
public:
    BRDF()
    {
        sampleNumber = SAMPLE_NUMBER;
    }

    void init(int band, BRDF_TYPE type);

    int band()
    {
        return _band;
    }

    int sampleNumber;
    Eigen::VectorXf** _BRDFlookupTable;

private:
    int _band;
};

#endif
