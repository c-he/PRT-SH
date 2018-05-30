#define _USE_MATH_DEFINES
#include <cmath>
#include <ctime>
#include <iostream>
#include <random>
#include "sampler.h"
#include "sphericalHarmonics.h"
#include "utils.h"

Sampler::Sampler(unsigned n)
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<float> u(0, 1);

    for (unsigned i = 0; i < n; ++i)
    {
        for (unsigned j = 0; j < n; ++j)
        {
            glm::vec2 spherical;
            glm::vec3 cartesian;

            float x = ((float)i + u(e)) / (float)n;
            float y = ((float)j + u(e)) / (float)n;
            // theta
            spherical[0] = 2.0f * acos(sqrt(1 - x));
            // phi
            spherical[1] = 2.0f * M_PI * y;
            // x
            cartesian.x = sin(spherical[0]) * cos(spherical[1]);
            // y
            cartesian.y = sin(spherical[0]) * sin(spherical[1]);
            // z
            cartesian.z = cos(spherical[0]);

            _samples.emplace_back(Sample(cartesian, spherical));
        }
    }
}

void Sampler::computeSH(int band)
{
    int band2 = band * band;
    size_t size = _samples.size();

    for (size_t i = 0; i < size; i++)
    {
        _samples[i]._SHvalue = new float[band2];
        for (int l = 0; l < band; l++)
        {
            for (int m = -l; m <= l; m++)
            {
                if (l == 1 && m == -1)
                {
                    //system("pause");
                }

                int index = l * (l + 1) + m;
                _samples[i]._SHvalue[index] = (float)SphericalH::SHvalue(
                    _samples[i]._sphericalCoord[0], _samples[i]._sphericalCoord[1], l, m);
                if (_isnan(_samples[i]._SHvalue[index]))
                {
                    std::cout << "NAN." << std::endl;
                    system("pause");
                }
                // std::cout << _samples[i]._SHvalue[index] << std::endl;
                if (fabs(_samples[i]._SHvalue[index]) > 1.0f)
                {
                    std::cout << "Value more than 1." << std::endl;
                    system("pause");
                }
            }
        }
        //std::cout << (float)i / _samples.size() << std::endl;
    }
}
