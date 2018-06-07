#ifndef GENERALOBJECT_H_
#define GENERALOBJECT_H_

#include <vector>
#include <Eigen/Dense>
#include <glm/glm.hpp>
#include "bvhTree.h"
#include "object.h"
#include "sampler.h"

class GeneralObject : public Object
{
public:
    GeneralObject()
    {
        _difforGeneral = true;
        _glossiness = 4.0f;
    }

    void project2SH(int mode, int band, int sampleNumber, int bounce) override;
    void write2Disk(std::string filename) override;
    void write2Diskbin(std::string filename) override;
    void readFDisk(std::string filename) override;
    void readFDiskbin(std::string filename) override;

    void setGlossiness(float glossiness)
    {
        _glossiness = glossiness;
    }

    // _TransferMatrix[0] for read and write, the other for read.
    std::vector<Eigen::MatrixXf> _TransferMatrix[3];

    std::vector<glm::vec4> _tangent;
    Eigen::VectorXf _BRDFcoeff;
    float _glossiness;

private:
    void glossyUnshadow(int size, int band2, Sampler* sampler, TransferType type, BVHTree* Inbvht = nullptr);
    void glossyShadow(int size, int band2, Sampler* sampler, TransferType type, BVHTree* Inbvht = nullptr);
    void glossyInterReflect(int size, int band2, Sampler* sampler, TransferType type, int bounce);

    void computeTBN();
    void computeBRDFKernel();
};

#endif
