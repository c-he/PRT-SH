#ifndef SHROTATION_H_
#define SHROTATION_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <Eigen/Dense>

/*
 * Some of these functions are partially adapted from Paul's Project.
 * For more information, please refer: http://www.paulsprojects.net/opengl/sh/sh.html
 */

inline void singleZMatrix(int band, float angle, Eigen::MatrixXf& res)
{
    // Calculate the size of the matrix.
    int size = 2 * band + 1;

    // @NOTE: in our implementation, the parameter angle is passed-in as radians.
    // Convert angle to radians.
    // angle *= M_PI / 180.0f;

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            // Initialize this entry to zero.
            res(i, j) = 0.0f;
            // For the central row, entry is 1 if j == i, else 0.
            if ((i == band) && (j == band))
            {
                res(i, j) = 1.0f;
            }
            // For i < band, entry is cos if j == i or sin if j == size - i - 1
            // The angle used is k*angle where k = band - i
            if (i < band)
            {
                int k = band - i;

                if (j == i)
                    res(i, j) = cos(k * angle);
                if (j == (size - i - 1))
                    res(i, j) = sin(k * angle);
            }
            // For i > band, entry is cos if j == i or -sin if j == size - i - 1
            // The angle used is k*angle where k = i - band
            if (i > band)
            {
                int k = i - band;

                if (j == i)
                    res(i, j) = cos(k * angle);
                if (j == (size - i - 1))
                    res(i, j) = -sin(k * angle);
            }
        }
    }
}

inline void ZRotateMatrix(int band, float angle, std::vector<Eigen::MatrixXf>& res)
{
    for (int i = 0; i < band; ++i)
    {
        int size = 2 * i + 1;
        Eigen::MatrixXf mtemp(size, size);

        singleZMatrix(i, angle, mtemp);
        res.push_back(mtemp);
    }
}

// Get a matrix for rotating one band of coefficients about the X axis by 90 degrees.
inline void singleX90DegreeMatrix(int band, Eigen::MatrixXf& res)
{
    res.setZero();
    if (band == 0)
    {
        res(0, 0) = 1.0f;
    }
    if (band == 1)
    {
        res(0, 1) = 1.0f;
        res(1, 0) = -1.0f;
        res(2, 2) = 1.0f;
    }
    if (band == 2)
    {
        res(0, 3) = 1.0;
        res(1, 1) = -1.0;
        res(2, 2) = -0.5;
        res(2, 4) = -sqrt(3.0) / 2.0;
        res(3, 0) = -1.0;
        res(4, 2) = -sqrt(3.0) / 2.0;
        res(4, 4) = 0.5;
    }
    if (band == 3)
    {
        res(0, 3) = -sqrt(0.625);
        res(0, 5) = sqrt(0.375);

        res(1, 1) = -1.0;

        res(2, 3) = -sqrt(0.375);
        res(2, 5) = -sqrt(0.625);

        res(3, 0) = sqrt(0.625);
        res(3, 2) = sqrt(0.375);

        res(4, 4) = -0.25;
        res(4, 6) = -sqrt(15.0) / 4.0;

        res(5, 0) = -sqrt(0.375);
        res(5, 2) = sqrt(0.625);

        res(6, 4) = -sqrt(15.0) / 4.0;
        res(6, 6) = 0.25;
    }
}

inline void XRotateMatrix(int band, std::vector<Eigen::MatrixXf>& res)
{
    for (int i = 0; i < band; ++i)
    {
        int size = 2 * i + 1;
        Eigen::MatrixXf mtemp(size, size);

        singleX90DegreeMatrix(i, mtemp);
        res.push_back(mtemp);
    }
}

inline void transfer(int band, std::vector<Eigen::MatrixXf>& m, Eigen::VectorXf& v, bool transpose = false)
{
    for (int i = 0; i < band; i++)
    {
        int offset = i * i;
        float* result = new float[2 * i + 1];
        int size = 2 * i + 1;

        for (int j = 0; j < size; j++)
        {
            // Clear this entry of out vector.
            result[j] = 0.0f;
            // Loop through matrix row/column.
            for (int k = 0; k < size; k++)
            {
                if (transpose)
                {
                    result[j] += m[i](k, j) * v(offset + k);
                }
                else
                {
                    result[j] += m[i](j, k) * v(offset + k);
                }
            }
        }

        for (int j = 0; j < size; j++)
            v(offset + j) = result[j];

        delete [] result;
    }
}

#endif
