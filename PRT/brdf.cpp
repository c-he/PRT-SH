#define _USE_MATH_DEFINES
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "brdf.h"
#include "sampler.h"

Sampler viewSampler(SAMPLE_NUMBER);
std::string band_name[] = {"constant", "linear", "quadratic", "cubic", "quartic"};

void BRDF::init(int band, BRDF_TYPE type)
{
    _band = band;
    int band2 = band * band;
    Sampler lightSampler(sampleNumber);
    lightSampler.computeSH(band);
    const int lightSampleNumber = lightSampler._samples.size();
    const float weight = 4.0f * M_PI / lightSampleNumber;
    _BRDFlookupTable = new Eigen::VectorXf*[sampleNumber];

    for (int i = 0; i < sampleNumber; i++)
    {
        _BRDFlookupTable[i] = new Eigen::VectorXf[sampleNumber];
        for (int j = 0; j < sampleNumber; j++)
        {
            _BRDFlookupTable[i][j].resize(band2);
            _BRDFlookupTable[i][j].setZero();

            Sample vsp = viewSampler._samples[i * sampleNumber + j];
            glm::vec3 n(0.0f, 1.0f, 0.0f);
            glm::vec3 v(1.0f, 0.0f, 0.0f);
            glm::vec3 u(0.0f, 0.0f, 1.0f);

            if (type == BRDF_PHONG)
            {
                // The naive version of Phong, ignoring spatial variance.
                const float diffuse_albedo = 1.2f;
                const int shininess = 4.0f;

                // Monte-Carlo integration for light directions.
                for (int k = 0; k < lightSampleNumber; k++)
                {
                    Sample lsp = lightSampler._samples[k];
                    float brdf;
                    if (vsp._sphericalCoord[0] >= M_PI / 2.0f || lsp._sphericalCoord[0] >= M_PI / 2.0f)
                    {
                        // The samping of BRDF is done for the upper hemisphere.
                        brdf = 0.0f;
                    }
                    else
                    {
                        // Naive phong.
                        glm::vec3 reflect = 2 * glm::dot(n, lsp._cartesCoord) * n - lsp._cartesCoord;
                        float specular = std::max(glm::dot(glm::normalize(reflect), glm::normalize(vsp._cartesCoord)),
                                                  0.0f);
                        brdf = diffuse_albedo / M_PI + powf(specular, shininess);

                        if (_isnan(brdf))
                        {
                            std::cout << "Phong: " << brdf << std::endl;
                            system("pause");
                        }
                    }
                    // Projection.
                    for (int l = 0; l < band2; l++)
                    {
                        _BRDFlookupTable[i][j](l) += lsp._SHvalue[l] * brdf * std::max(0.0f, lsp._cartesCoord.z);
                    }
                }
            }

            if (type == BRDF_WARD_ISOTROPIC)
            {
                // Measured BRDF using the Isotropic Gaussian Model in Ward's paper.
                /*
                 * White ceramic tile:
                 * diffuse albedo             = .70
                 * specular albedo            = .050
                 * alpha (standard deviation) = .071
                 */
                const float diffuse_albedo = .70f;
                const float specular_albedo = .050f;
                const float alpha = .071f;

                for (int k = 0; k < lightSampleNumber; k++)
                {
                    Sample lsp = lightSampler._samples[k];
                    glm::vec3 h = glm::normalize(vsp._cartesCoord + lsp._cartesCoord);
                    float delta = acos(glm::dot(h, n));

                    float brdf;
                    if (vsp._sphericalCoord[0] >= M_PI / 2.0f || lsp._sphericalCoord[0] >= M_PI / 2.0f)
                    {
                        // The samping of BRDF is done for the upper hemisphere.
                        brdf = 0.0f;
                    }
                    else
                    {
                        float factor1 = 1.0f / sqrt(cos(lsp._sphericalCoord[0]) * cos(vsp._sphericalCoord[0]));
                        float factor2 = exp(-pow(tan(delta), 2) / pow(alpha, 2)) / (4.0f * M_PI * pow(alpha, 2));
                        brdf = diffuse_albedo / M_PI + specular_albedo * factor1 * factor2;

                        if (_isnan(brdf))
                        {
                            std::cout << "Ward Isotropic: " << brdf << std::endl;
                            system("pause");
                        }
                    }
                    // Projection.
                    for (int l = 0; l < band2; l++)
                    {
                        _BRDFlookupTable[i][j](l) += lsp._SHvalue[l] * brdf * std::max(0.0f, lsp._cartesCoord.z);
                    }
                }
            }

            if (type == BRDF_WARD_ANISOTROPIC)
            {
                // Measured BRDF using the Anisotropic (Elliptical) Gaussian Model.
                /*
                * Barnished plywood:
                * diffuse albedo  = .33
                * specular albedo = .025
                * alpha x         = .04
                * alpha y         = .11
                */
                const float diffuse_albedo = .33f;
                const float specular_albedo = .025f;
                const float alpha_x = .04f;
                const float alpha_y = .11f;

                for (int k = 0; k < lightSampleNumber; k++)
                {
                    Sample lsp = lightSampler._samples[k];
                    glm::vec3 h = glm::normalize(vsp._cartesCoord + lsp._cartesCoord);
                    glm::vec3 h_proj = glm::normalize(h - glm::dot(h, n) * n);
                    float delta = acos(glm::dot(h, n));
                    float cosine2_phi = powf(glm::dot(v, h_proj), 2);
                    float sine2_phi = 1.0f - cosine2_phi;

                    float brdf;
                    if (vsp._sphericalCoord[0] >= M_PI / 2.0f || lsp._sphericalCoord[0] >= M_PI / 2.0f)
                    {
                        // The samping of BRDF is done for the upper hemisphere.
                        brdf = 0.0f;
                    }
                    else
                    {
                        float factor1 = 1.0f / sqrt(cos(lsp._sphericalCoord[0]) * cos(vsp._sphericalCoord[0]));
                        float factor2 = exp(
                            -pow(tan(delta), 2) * (cosine2_phi / powf(alpha_x, 2) + sine2_phi / pow(alpha_y, 2))) / (
                            4.0f * M_PI * alpha_x * alpha_y);
                        brdf = diffuse_albedo / M_PI + specular_albedo * factor1 * factor2;

                        if (_isnan(brdf))
                        {
                            std::cout << "Ward Anisotropic: " << brdf << std::endl;
                            system("pause");
                        }
                    }
                    // Projection.
                    for (int l = 0; l < band2; l++)
                    {
                        _BRDFlookupTable[i][j](l) += lsp._SHvalue[l] * brdf * std::max(0.0f, lsp._cartesCoord.z);
                    }
                }
            }

            // Normalization.
            for (int k = 0; k < band2; k++)
            {
                _BRDFlookupTable[i][j](k) = _BRDFlookupTable[i][j](k) * weight;
            }
        }
    }

#ifdef SHOW_BRDF
    cv::Mat brdf(sampleNumber * 2, sampleNumber * 2, CV_32FC1);
    for (int i = 0; i < sampleNumber * 2; i++)
    {
        for (int j = 0; j < sampleNumber * 2; j++)
        {
            // Just parameterize the upper hemisphere.
            brdf.at<float>(i * sampleNumber * 2 + j) = _BRDFlookupTable[i / 4][j / 2].squaredNorm();
        }
    }
    cv::Mat1b brdf_8UC1;
    brdf.convertTo(brdf_8UC1, CV_8UC1, 255);
    // cv::imshow("brdf", brdf);
    switch (type)
    {
    case BRDF_PHONG:
        cv::imwrite("brdf/PHONG_" + band_name[band - 1] + ".jpg", brdf_8UC1);
        break;
    case BRDF_WARD_ISOTROPIC:
        cv::imwrite("brdf/WARD_ISOTROPIC_" + band_name[band - 1] + ".jpg", brdf_8UC1);
        break;
    case BRDF_WARD_ANISOTROPIC:
        cv::imwrite("brdf/WARD_ANISOTROPIC_" + band_name[band - 1] + ".jpg", brdf_8UC1);
        break;
    default:
        break;
    }
    cvWaitKey(0);
#endif
}
