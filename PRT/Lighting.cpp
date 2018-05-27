#include "Lighting.h"
#include <fstream>
#include <iostream>
#include <CImg/CImg.h>
#include "rgbe.h"

Lighting::Lighting(string path, LightType type, int band) //construction function for process
{
    std::cout << "processs init." << std::endl;
    _path = path;
    _ltype = type;
    _band = band;

    /*cimg_library::CImg<unsigned char> src(_path.c_str());
    _width = src.width();
    _height = src.height();

    _pixels = new vec3[_width * _height];

    for(int i = 0;i < _height;++i)
        for(int j = 0;j < _width;++j)
        {
            _pixels[i * _width + j] = 
            vec3(
                    src(j,i,0,0),
                    src(j,i,0,1),
                    src(j,i,0,2)
                );
            
        }*/
    // Loading hdr textures.
    _Format = GL_RGB;
    _Type = GL_FLOAT;
    _InternalFormat = GL_RGB16F_ARB;

    std::cout << "Loading HDR texture: " << path << std::endl;

    FILE* file = fopen(path.c_str(), "rb");
    RGBE_ReadHeader(file, &_width, &_height, NULL);
    _data = new float[3 * _width * _height];
    RGBE_ReadPixels_RLE(file, _data, _width, _height);

    fclose(file);

    std::cout << " - WIDTH                    : " << _width << std::endl;
    std::cout << " - HEIGHT                   : " << _height << std::endl;
    std::cout << " - MEMORY SIZE              : " << (3 * _width * _height * sizeof(float)) / 8 << " bytes" << std::
        endl;
    std::cout << " - FORMAT                   : " << ((_Format == GL_RGB) ? "RGB" : "") << std::endl;
    std::cout << " - TYPE                     : " << ((_Type == GL_FLOAT) ? "Float" : "") << std::endl;
    std::cout << " - INTERNAL FORMAT          : " << ((_InternalFormat == GL_RGB8) ? "RGB8" : "RGB16F") << std::endl;
}

Lighting::~Lighting()
{
    delete[] _data;
}


void Lighting::init(string CoeffPath, vec3 HDRaffect, vec3 Glossyaffect)
{
    _HDRaffect = HDRaffect;
    _Glossyaffect = Glossyaffect;
    std::cout << "runtime init." << std::endl;
    std::ifstream in(CoeffPath, std::ifstream::binary);
    //std::ifstream in(CoeffPath);
    vec3 temp;

    in.read((char *)&_band, sizeof(int));
    //in >> _band ;
    int band2 = _band * _band;
    _coeffs.clear();

    for (int i = 0; i < 3; ++i)
    {
        _Vcoeffs[i].resize(band2);
        _Vcoeffs[i].setZero();
    }

    for (int i = 0; i < band2; ++i)
    {
        /*in	>> temp.x 
            >> temp.y 
            >> temp.z ;*/

        in.read((char *)&temp.x, sizeof(float));
        in.read((char *)&temp.y, sizeof(float));
        in.read((char *)&temp.z, sizeof(float));

        _Vcoeffs[0](i) = temp.x;
        _Vcoeffs[1](i) = temp.y;
        _Vcoeffs[2](i) = temp.z;

        _coeffs.push_back(temp);
    }

    in.close();
}

//return the light color 
vec3 Lighting::probeColor(vec3 dir)
{
    float d = sqrt(dir.x * dir.x + dir.y * dir.y);

    float r;
    if (fabs(d) <= M_ZERO)
    {
        r = 0.0f;
    }
    else
    {
        r = (1.0f / (2.0f * (float)MY_PI)) * acos(dir.z) / d;
    }

    vec2 texCoord;
    texCoord.x = 0.5f + dir.x * r;
    texCoord.y = 0.5f + dir.y * r;

    glm::ivec2 pixelCoord;
    pixelCoord.x = (int)(_width * texCoord.x);
    pixelCoord.y = (int)(_height * (1.0f - texCoord.y));

    int index = pixelCoord.y * _width + pixelCoord.x;

    int offset = 3 * index;

    return vec3(_data[offset], _data[offset + 1], _data[offset + 2]);

    return _pixels[index];
}


void Lighting::process(int sampleNumber, bool image = true)
{
    _band = 4;
    int sqrtnum = (int)sqrt(sampleNumber);
    int band2 = _band * _band;
    float weight = 4.0f * (float)MY_PI / sampleNumber;
    Sampler stemp(sqrtnum);

    stemp.computeSH(_band);
    //system("pause");
    _coeffs.clear();
    _coeffs.resize(band2, vec3(0.0f, 0.0f, 0.0f));


    for (int i = 0; i < sampleNumber; ++i)
    {
        vec3 dir = stemp._samples[i]._cartesCoord;
        for (int j = 0; j < band2; ++j)
        {
            float sh_value = stemp._samples[i]._SHvalue[j];
            if (image)
            {
                vec3 color = probeColor(dir);
                _coeffs[j] += color * sh_value;
            }
            else
            {
                _coeffs[j] += sh_value * Simplelight(stemp._samples[i]._sphericalCoord[0],
                                                     stemp._samples[i]._sphericalCoord[1]);
            }
        }

        //std::cout << ((float)i/sampleNumber) << std::endl;
    }

    for (int i = 0; i < band2; ++i)
    {
        _coeffs[i] = _coeffs[i] * weight;
    }
}

//void Lighting::rotateZYZ(mat4 rM)//rM is rotation matrix
void Lighting::rotateZYZ(vector<vec2>& para)
{
    int band2 = _band * _band;
    float theta, phi;
    //theta *= (float)M_PI/180.0f;
    //phi *= (float)M_PI/180.0f;

    for (int i = 0; i < band2; ++i)
        for (int j = 0; j < 3; ++j)
        {
            _Vcoeffs[j](i) = _coeffs[i][j];
        }

    for (unsigned i = 0; i < para.size(); ++i)
    {
        theta = para[i].x;
        phi = para[i].y;

        vector<MatrixXf> X90;
        XRotateMatrix(_band, X90);
        transfer(_band, X90, _Vcoeffs[0]);
        transfer(_band, X90, _Vcoeffs[1]);
        transfer(_band, X90, _Vcoeffs[2]);


        vector<MatrixXf> Zalpha;
        ZRotateMatrix(_band, theta, Zalpha);
        transfer(_band, Zalpha, _Vcoeffs[0]);
        transfer(_band, Zalpha, _Vcoeffs[1]);
        transfer(_band, Zalpha, _Vcoeffs[2]);

        transfer(_band, X90, _Vcoeffs[0], true);
        transfer(_band, X90, _Vcoeffs[1], true);
        transfer(_band, X90, _Vcoeffs[2], true);

        vector<MatrixXf> Zbeta;
        ZRotateMatrix(_band, phi, Zbeta);
        transfer(_band, Zbeta, _Vcoeffs[0]);
        transfer(_band, Zbeta, _Vcoeffs[1]);
        transfer(_band, Zbeta, _Vcoeffs[2]);
    }
}

void Lighting::write2Disk(string outFile)
{
    std::ofstream out(outFile, std::ofstream::binary);
    out.write((char *)&_band, sizeof(int));
    int band2 = _band * _band;
    for (int i = 0; i < band2; ++i)
    {
        /*	out << _coeffs[i].x << ' '
                << _coeffs[i].y << ' '
                << _coeffs[i].z << ' '
                << std::endl;*/
        out.write((char *)&_coeffs[i].x, sizeof(float));
        out.write((char *)&_coeffs[i].y, sizeof(float));
        out.write((char *)&_coeffs[i].z, sizeof(float));
    }
    out.close();
}
