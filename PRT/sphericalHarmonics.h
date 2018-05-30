#ifndef SPHERICALHARMONICS_H_
#define SPHERICALHARMONICS_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdlib>
#include "utils.h"

namespace SphericalH
{
    // Normalization constants.
    double static Kvalue(int l, int m)
    {
        if (m == 0)
        {
            return sqrt((2 * l + 1) / (4 * M_PI));
        }

        double up = (2 * l + 1) * factorial(l - abs(m));
        double down = (4 * M_PI) * factorial(l + abs(m));

        return sqrt(up / down);
    }

    // Value for Associated Legendre Polynomial.
    // Some information about Associated Legendre Polynomial:
    // https://en.wikipedia.org/wiki/Associated_Legendre_polynomials?oldformat=true
    double static evaluateLegendre(double x, int l, int m)
    {
        double result = 0.0;
        if (l == m)
        {
            result = minusOnePower(m) * doubleFactorial(2 * m - 1) * pow((1 - x * x), m / 2.0);
        }
        else if (l == m + 1)
        {
            result = x * (2 * m + 1) * evaluateLegendre(x, m, m);
        }
        else
        {
            result = (x * (2 * l - 1) * evaluateLegendre(x, l - 1, m) - (l + m - 1) * evaluateLegendre(x, l - 2, m)) /
                (l - m);
        }
        return result;
    }

    // Value for Spherical Harmonic.
    double static SHvalue(double theta, double phi, int l, int m)
    {
        double result = 0.0;
        if (m == 0)
        {
            result = Kvalue(l, 0) * evaluateLegendre(cos(theta), l, 0);
        }
        else if (m > 0)
        {
            result = sqrt(2.0f) * Kvalue(l, m) * cos(m * phi) * evaluateLegendre(cos(theta), l, m);
        }
        else
        {
            result = sqrt(2.0f) * Kvalue(l, m) * sin(-m * phi) * evaluateLegendre(cos(theta), l, -m);
        }

        if (fabs(result) <= M_ZERO)
            result = 0.0;

        if (_isnan(result))
        {
            std::cout << "SPHERICAL HARMONIC NAN" << std::endl;
            std::cout << "theta: " << theta << " " << "phi: " << phi << std::endl;
        }
        return result;
    }
};

#endif
