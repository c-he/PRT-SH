#define _USE_MATH_DEFINES
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include "brdf.h"
#include "sampler.h"

void BRDF::init(int band, BRDF_TYPE type)
{
    int band2 = band * band;
    Sampler viewSampler(sampleNumber);

    if (type == Phong)
    {
        // The naive version of Phong, ignoring spatial variance.
        Sampler lightSampler(sampleNumber);
        lightSampler.computeSH(band);
        int lightSampleNumber = lightSampler._samples.size();
        float weight = 4.0f * M_PI / lightSampleNumber;
        glm::vec3 normal(0.0f, 1.0f, 0.0f);
        const float diffuse_albedo = 0.15f;
        const int shininess = 4.0f;
        _BRDFlookupTable = new Eigen::VectorXf*[sampleNumber];

        for (int i = 0; i < sampleNumber; i++)
        {
            _BRDFlookupTable[i] = new Eigen::VectorXf[sampleNumber];
            for (int j = 0; j < sampleNumber; j++)
            {
                _BRDFlookupTable[i][j].resize(band2);
                _BRDFlookupTable[i][j].setZero();

                // Monte-Carlo integration for light directions.
                for (int k = 0; k < lightSampleNumber; k++)
                {
                    Sample sp = lightSampler._samples[k];
                    // Naive cosine.
                    float specular = std::max(glm::dot(normal, glm::normalize(sp._cartesCoord)), 0.0f);
                    float brdf = diffuse_albedo / M_PI + powf(specular, shininess);
                    // Projection.
                    for (int l = 0; l < band2; l++)
                    {
                        _BRDFlookupTable[i][j](l) += sp._SHvalue[l] * brdf * std::max(0.0f, sp._cartesCoord.z);
                    }
                }
                // Normalization.
                for (int k = 0; k < band2; k++)
                {
                    _BRDFlookupTable[i][j](k) = _BRDFlookupTable[i][j](k) * weight;
                }
            }
        }
    }
}
