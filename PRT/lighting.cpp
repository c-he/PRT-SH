#include <cmath>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <glm/gtx/string_cast.hpp>
#include "lighting.h"
#include "rgbe.h"
#include "sampler.h"
#include "shRotation.h"
#include "simpleLighting.h"
#include "utils.h"

extern std::string bands[];

// Constructor for preprocessing.
Lighting::Lighting(std::string path, LightType type, int band):
    _path(path), _ltype(type), _band(band)
{
    if (path.empty())
    {
        std::cout << "Lighting probe: simple light" << std::endl;
    }
    else
    {
        size_t beginIndex = path.rfind('\\');
        size_t endIndex = path.rfind('.');
        // std::cout << "begin = " << beginIndex << "\end = " << endIndex << std::endl;
        _filename = path.substr(beginIndex + 1, endIndex - beginIndex - 1);
        std::cout << "Lighting probe: " << _filename << std::endl;
        // Loading hdr textures.
        std::cout << "Loading HDR texture: " << path << std::endl;

        FILE* file = fopen(path.c_str(), "rb");
        RGBE_ReadHeader(file, &_width, &_height, NULL);
        _data = new float[3 * _width * _height];
        RGBE_ReadPixels_RLE(file, _data, _width, _height);

        fclose(file);
    }
}

Lighting::Lighting(int band, Eigen::VectorXf coeffs[3])
{
    _band = band;
    int band2 = band * band;

    // RGB channel.
    for (size_t i = 0; i < 3; i++)
    {
        _Vcoeffs[i].resize(band2);
        // SH coefficients.
        for (size_t j = 0; j < band2; j++)
            _Vcoeffs[i](j) = coeffs[i](j);
    }

    for (size_t i = 0; i < band2; i++)
        _coeffs.push_back(glm::vec3(coeffs[0](i), coeffs[1](i), coeffs[2](i)));
}

Lighting::~Lighting()
{
    delete[] _data;
}

void Lighting::init(std::string coeffPath, glm::vec3 hdrEffect, glm::vec3 glossyEffect)
{
    _hdrEffect = hdrEffect;
    _glossyEffect = glossyEffect;
    std::cout << "Lighting probe: " << coeffPath << std::endl;
    std::ifstream in(coeffPath, std::ifstream::binary);
    glm::vec3 temp;

    in.read((char *)&_band, sizeof(int));
    std::cout << "band = " << _band << std::endl;
    int band2 = _band * _band;
    _coeffs.clear();

    for (size_t i = 0; i < 3; i++)
    {
        _Vcoeffs[i].resize(band2);
        _Vcoeffs[i].setZero();
    }

    for (size_t i = 0; i < band2; i++)
    {
        in.read((char *)&temp.x, sizeof(float));
        in.read((char *)&temp.y, sizeof(float));
        in.read((char *)&temp.z, sizeof(float));

        _Vcoeffs[0](i) = temp.x;
        _Vcoeffs[1](i) = temp.y;
        _Vcoeffs[2](i) = temp.z;

        _coeffs.push_back(temp);
        // std::cout << temp.x << " " << temp.y << " " << temp.z << std::endl;
    }

    in.close();
}

// Return the light color.
// For more information about the light probe images we use:
// http://www.pauldebevec.com/Probes/
glm::vec3 Lighting::probeColor(glm::vec3 dir)
{
    dir = glm::normalize(dir);
    float d = sqrt(dir.x * dir.x + dir.y * dir.y);

    float r;
    if (fabs(d) <= M_ZERO)
    {
        r = 0.0f;
    }
    else
    {
        r = (1.0f / (2.0f * M_PI)) * acos(dir.z) / d;
    }

    glm::vec2 texCoord;
    texCoord.x = 0.5f + dir.x * r;
    texCoord.y = 0.5f + dir.y * r;

    glm::ivec2 pixelCoord;
    pixelCoord.x = (int)(_width * texCoord.x);
    pixelCoord.y = (int)(_height * (1.0f - texCoord.y));

    int index = pixelCoord.y * _width + pixelCoord.x;

    int offset = 3 * index;

    return glm::vec3(_data[offset], _data[offset + 1], _data[offset + 2]);
}

// Compute incident lighting at one or more sample points near object in terms of the SH basis.
void Lighting::process(int sampleNumber, bool image)
{
    int sqrtnum = (int)sqrt(sampleNumber);
    int band2 = _band * _band;
    // @NOTE: this weight comes from the integral of solid angle ds, referred to section 6.2 in this paper.
    float weight = 4.0f * M_PI / sampleNumber;
    Sampler stemp(sqrtnum);

    stemp.computeSH(_band);
    _coeffs.clear();
    _coeffs.resize(band2, glm::vec3(0.0f, 0.0f, 0.0f));

    // For one channel: sampleNumber-dimension vector -> band2-dimension vector
    for (int i = 0; i < sampleNumber; i++)
    {
        glm::vec3 dir = stemp._samples[i]._cartesCoord;
        for (int j = 0; j < band2; j++)
        {
            float SHvalue = stemp._samples[i]._SHvalue[j];
            if (image)
            {
                glm::vec3 color = probeColor(dir);
                _coeffs[j] += color * SHvalue;
            }
            else
            {
                _coeffs[j] += SHvalue * Simplelight(stemp._samples[i]._sphericalCoord[0],
                                                    stemp._samples[i]._sphericalCoord[1]);
            }
        }
    }

    for (int i = 0; i < band2; ++i)
    {
        _coeffs[i] = _coeffs[i] * weight;
        // std::cout << glm::to_string(_coeffs[i]) << std::endl;
    }

    // Bake an img_length * img_length image to visualize.
    const int img_length = 120;
    const int band_length = 120 / _band;
    cv::Mat red(img_length, img_length, CV_32FC1);
    cv::Mat green(img_length, img_length, CV_32FC1);
    cv::Mat blue(img_length, img_length, CV_32FC1);
    cv::Mat img = cv::Mat::zeros(img_length, img_length, CV_32FC3);
    for (int i = 0; i < img_length; i++)
    {
        for (int j = 0; j < img_length; j++)
        {
            // Opencv uses BGR color format.
            blue.at<float>(i * img_length + j) = _coeffs[(i / band_length) * _band + (j / band_length)].b;
            green.at<float>(i * img_length + j) = _coeffs[(i / band_length) * _band + (j / band_length)].g;
            red.at<float>(i * img_length + j) = _coeffs[(i / band_length) * _band + (j / band_length)].r;
        }
    }
    std::vector<cv::Mat> channels = {blue, green, red};
    cv::merge(channels, img);
    cv::Mat3b img_8UC3;
    img.convertTo(img_8UC3, CV_8UC3, 255);
    // imshow("image", img_8UC3);
    if (image)
    {
        cv::imwrite("../../PRT/processedData/lightings/" + bands[_band - 2] + "/" + _filename + ".jpg", img_8UC3);
    }
    else
    {
        cv::imwrite("../../PRT/processedData/lightings/" + bands[_band - 2] + "/simple_probe.jpg", img_8UC3);
    }
    cvWaitKey(0);
}

// Rotating SH coefficients.
void Lighting::rotateZYZ(std::vector<glm::vec2>& para)
{
    int band2 = _band * _band;
    float theta, phi;

    for (int i = 0; i < band2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            _Vcoeffs[j](i) = _coeffs[i][j];
        }
    }

    for (size_t i = 0; i < para.size(); i++)
    {
        theta = para[i].x;
        phi = para[i].y;

        std::vector<Eigen::MatrixXf> X90;
        XRotateMatrix(_band, X90);
        transfer(_band, X90, _Vcoeffs[0]);
        transfer(_band, X90, _Vcoeffs[1]);
        transfer(_band, X90, _Vcoeffs[2]);


        std::vector<Eigen::MatrixXf> Zalpha;
        ZRotateMatrix(_band, theta, Zalpha);
        transfer(_band, Zalpha, _Vcoeffs[0]);
        transfer(_band, Zalpha, _Vcoeffs[1]);
        transfer(_band, Zalpha, _Vcoeffs[2]);

        transfer(_band, X90, _Vcoeffs[0], true);
        transfer(_band, X90, _Vcoeffs[1], true);
        transfer(_band, X90, _Vcoeffs[2], true);

        std::vector<Eigen::MatrixXf> Zbeta;
        ZRotateMatrix(_band, phi, Zbeta);
        transfer(_band, Zbeta, _Vcoeffs[0]);
        transfer(_band, Zbeta, _Vcoeffs[1]);
        transfer(_band, Zbeta, _Vcoeffs[2]);
    }
}

void Lighting::write2Diskbin(std::string outFile)
{
    std::ofstream out(outFile, std::ofstream::binary);
    out.write((char *)&_band, sizeof(int));
    int band2 = _band * _band;
    for (int i = 0; i < band2; i++)
    {
        out.write((char *)&_coeffs[i].x, sizeof(float));
        out.write((char *)&_coeffs[i].y, sizeof(float));
        out.write((char *)&_coeffs[i].z, sizeof(float));
    }
    out.close();
    std::cout << "Lighting probe generated." << std::endl;
}
