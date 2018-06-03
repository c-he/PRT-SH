#ifndef DIFFUSEOBJECT_H_
#define DIFFUSEOBJECT_H_

#include "bvhTree.h"
#include "sampler.h"
#include "object.h"

class DiffuseObject : public Object
{
public:
    void project2SH(int mode, int band, int sampleNumber, int bounce) override;
    void write2Disk(std::string filename) override;
    void write2Diskbin(std::string filename) override;
    void readFDisk(std::string filename) override;
    void readFDiskbin(std::string filename) override;
    // For read.
    std::vector<std::vector<glm::vec3>> _DTransferFunc[3];

private:

    void diffuseUnshadow(int size, int band2, Sampler* sampler, TransferType type, BVHTree* Inbvht = nullptr);
    void diffuseShadow(int size, int band2, Sampler* sampler, TransferType type, BVHTree* Inbvht = nullptr);
    void diffuseInterreflect(int size, int band2, Sampler* sampler, TransferType type, int bounce);
    // For write.
    std::vector<std::vector<glm::vec3>> _TransferFunc;
};

#endif
