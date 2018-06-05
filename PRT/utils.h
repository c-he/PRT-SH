#ifndef UTILS_H_
#define UTILS_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#define M_ZERO 1e-9
#define M_DELTA 1e-6f

struct Triangle
{
    glm::vec3 _v0, _v1, _v2;
    int _index;

    Triangle()
    {
        _index = -1;
    }

    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, int index)
        : _v0(v0), _v1(v1), _v2(v2), _index(index)
    {
    }
};

struct Ray
{
    // Origin.
    glm::vec3 _o;
    // Direction.
    glm::vec3 _dir;
    glm::vec3 _inv;
    // Intersection parameter.
    double _tmin;
    double _tmax;
    double _t;
    int _index;

    Ray() = default;
    // Set tmin for avoiding the "epsilon problem" or the shadow acne problem.
    Ray(glm::vec3 o, glm::vec3 d, double tmin = M_DELTA, double tmax = DBL_MAX)
        : _o(o), _dir(d), _tmin(tmin), _tmax(tmax)
    {
        _inv.x = 1.0f / _dir.x;
        _inv.y = 1.0f / _dir.y;
        _inv.z = 1.0f / _dir.z;

        _t = DBL_MAX;
        // Normalize.
        _dir = glm::normalize(_dir);
    }
};

inline float Trimax(float a, float b, float c)
{
    return std::max(std::max(a, b), std::max(a, c));
}

inline float Trimin(float a, float b, float c)
{
    return std::min(std::min(a, b), std::min(a, c));
}

inline double factorial(int n)
{
    if (n <= 1)
        return 1.0;

    double result = 1.0;
    for (int i = 1; i <= n; ++i)
    {
        result *= i;
    }

    if (_isnan(result))
    {
        std::cout << "Factorial Error." << std::endl;
        system("pause");
    }

    return result;
}

inline double doubleFactorial(int n)
{
    if (n <= 1)
        return 1.0;

    double result = 1.0;
    for (unsigned i = n; i > 1; i -= 2)
    {
        result *= i;
    }


    if (_isnan(result))
    {
        std::cout << "Double Factorial Error." << std::endl;
        system("pause");
    }

    return result;
}

inline int minusOnePower(int n)
{
    if (n % 2)
    {
        return -1;
    }

    return 1;
}

/*
 * Compute barycentric coordinates (u, v, w) for point pc with respect to triangle (p[0], p[1], p[2]).
 * For more information, please refer: Real-Time Collision Detection, Christer Ericson, pp. 47-48
 */
inline void barycentric(glm::vec3 pc, glm::vec3 p[3], float& u, float& v, float& w)
{
    glm::vec3 v0 = p[1] - p[0];
    glm::vec3 v1 = p[2] - p[0];
    glm::vec3 v2 = pc - p[0];

    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    if (fabs(denom) < M_ZERO)
    {
        // std::cout << "Three vertices are colinear." << std::endl;
        // for (int i = 0; i < 3; i++)
        // {
        //     std::cout << "p[" << i << "] = " << glm::to_string(p[i]) << std::endl;
        // }
        // std::cout << "pc = " << glm::to_string(pc) << std::endl;

        u = v = w = 1.0f / 3.0f;
        return;
    }

    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
}

/*
 * Moller–Trumbore intersection algorithm for ray triangle intersection.
 * For more information, please refer:
 * https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm?oldformat=true
 */
inline bool rayTriangle(Ray& ray, Triangle& in)
{
    glm::vec3 e1 = in._v1 - in._v0;
    glm::vec3 e2 = in._v2 - in._v0;
    glm::vec3 p = glm::cross(ray._dir, e2);
    float a = glm::dot(e1, p);
    if (a < M_ZERO)
    {
        return false;
    }
    float f = 1.0f / a;
    glm::vec3 s = ray._o - in._v0;
    float beta = f * glm::dot(s, p);
    if (beta < 0.0f || beta > 1.0f)
    {
        return false;
    }

    glm::vec3 q = glm::cross(s, e1);
    float gamma = f * glm::dot(ray._dir, q);
    if (gamma < 0.0f || beta + gamma > 1.0f)
    {
        return false;
    }

    float t = f * glm::dot(e2, q);
    if (t >= ray._tmin && t <= ray._tmax)
    {
        // Hit, record the hit parameter and the triangle index.
        ray._t = t;
        ray._index = in._index;
        return true;
    }
    return false;
}

/*
 * @input parameters: sin(theta), cos(theta)
 * @output parameter: theta
 * @description: use Trigonometric functions to calculate the corresponding value in [0, 2PI].
 */
inline float inverseSC(float sinV, float cosV)
{
    cosV = glm::clamp(cosV, -1.0f, 1.0f);
    float result = acos(cosV);
    if (sinV < 0)
        result = 2.0f * M_PI - result;
    return result;
}

inline void rotateMatrixtoZYZ(float mat[3][3], float& alpha, float& beta, float& gamma)
{
    beta = acos(glm::clamp(mat[2][2], -1.0f, 1.0f));
    float sinbeta = sin(beta);
    if ((fabs(mat[1][2]) + fabs(mat[0][2]) + fabs(mat[2][0]) + fabs(mat[2][1])) < M_ZERO)
    {
        if (mat[2][2] == -1)
        {
            beta = M_PI;
            gamma = 0.0f;
            alpha = atan2(mat[2][0], mat[0][0]);
        }
        else
        {
            beta = 0.0f;
            gamma = 0.0f;
            alpha = inverseSC(mat[0][2], mat[0][0]);
        }
    }
    else
    {
        float cosalpha = mat[0][2] / sinbeta;
        float sinalpha = mat[1][2] / sinbeta;

        alpha = inverseSC(sinalpha, cosalpha);

        float cosgamma = -mat[2][0] / sinbeta;
        float singamma = mat[2][1] / sinbeta;

        gamma = inverseSC(singamma, cosgamma);
    }
}

inline void rotateMatrixtoYZY(float mat[3][3], float& alpha, float& beta, float& gamma)
{
    beta = acos(glm::clamp(mat[1][1], -1.0f, 1.0f));
    float sinbeta = sin(beta);
    if ((fabs(mat[0][1]) + fabs(mat[1][0]) + fabs(mat[1][2]) + fabs(mat[2][1])) < M_ZERO)
    {
        if (mat[1][1] == -1)
        {
            beta = M_PI;
            gamma = 0.0f;
            alpha = atan2(mat[2][0], mat[0][0]);
        }
        else
        {
            beta = 0.0f;
            gamma = 0.0f;
            alpha = inverseSC(mat[0][2], mat[0][0]);
        }
    }
    else
    {
        float cosalpha = -mat[0][1] / sinbeta;
        float sinalpha = mat[2][1] / sinbeta;

        alpha = inverseSC(sinalpha, cosalpha);

        float cosgamma = mat[1][0] / sinbeta;
        float singamma = mat[1][2] / sinbeta;

        gamma = inverseSC(singamma, cosgamma);
    }
}

inline void rotateMatrixtoXYX(float mat[3][3], float& alpha, float& beta, float& gamma)
{
    beta = acos(glm::clamp(mat[0][0], -1.0f, 1.0f));
    float sinbeta = sin(beta);
    if ((fabs(mat[0][1]) + fabs(mat[0][2]) + fabs(mat[1][0]) + fabs(mat[2][0])) < M_ZERO)
    {
        if (mat[0][0] == -1)
        {
            beta = M_PI;
            gamma = 0.0f;
            alpha = atan2(mat[2][0], mat[0][0]);
        }
        else
        {
            beta = 0.0f;
            gamma = 0.0f;
            alpha = atan2(mat[0][2], mat[0][0]);
        }
    }
    else
    {
        float cosalpha = -mat[0][2] / sinbeta;
        float sinalpha = mat[0][1] / sinbeta;

        alpha = inverseSC(sinalpha, cosalpha);

        float cosgamma = mat[0][2] / sinbeta;
        float singamma = mat[0][1] / sinbeta;

        gamma = inverseSC(singamma, cosgamma);
    }
}

#endif
