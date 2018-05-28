#include "sampler.h"

Sampler::Sampler(unsigned n)
{
    srand((unsigned)time(NULL));

    for (unsigned i = 0; i < n; ++i)
    {
        for (unsigned j = 0; j < n; ++j)
        {
            vec2 spherical;
            vec3 cartesian;

            float x = ((float)i + random0to1(n * n)) / (float)n;
            float y = ((float)j + random0to1(n * n)) / (float)n;
            spherical[0] = 2.0f * acos(sqrt(1 - x));
            spherical[1] = 2.0f * (float)MY_PI * y;


            cartesian[2] = sin(spherical[0]) * cos(spherical[1]);
            cartesian[0] = sin(spherical[0]) * sin(spherical[1]);
            cartesian[1] = cos(spherical[0]);


            _samples.push_back(Sample(cartesian, spherical));
        }
    }
}

void Sampler::computeSH(int band)
{
    int band2 = band * band;
    unsigned size = _samples.size();
    for (unsigned i = 0; i < size; ++i)
    {
        _samples[i]._SHvalue = new float[band2];
        for (int l = 0; l < band; ++l)
        {
            for (int m = -l; m <= l; ++m)
            {
                if (l == 1 && m == -1)
                {
                    //system("pause");
                }

                int index = l * (l + 1) + m;
                _samples[i]._SHvalue[index] = (float)
                    SphericalH::SHvalue(
                        _samples[i]._sphericalCoord[0],
                        _samples[i]._sphericalCoord[1], l, m
                    );
                if (_isnan(_samples[i]._SHvalue[index]))
                {
                    system("pause");
                    std::cout << "NAN." << std::endl;
                }
                //	std::cout << _samples[i]._SHvalue[index] << std::endl;
                if (fabs(_samples[i]._SHvalue[index]) > 1)
                {
                    system("pause");
                    std::cout << "Value more than 1." << std::endl;
                }
            }
        }
        //std::cout << (float)i / _samples.size() << std::endl;
    }
}
