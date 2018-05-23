#ifndef SPHERICALHARMONIC_H_
#define SPHERICALHARMONIC_H_

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <math.h>

#include "Sampler.h"
#include "toolFunction.h"

namespace SphericalH
{
	//normalization constants
	double static Kvalue(int l, int m)
	{
		if(m == 0)
		{
			return (double)sqrt((2 * l + 1)/(4 * MY_PI));
		}

		double up = (2 * l + 1) * factorial(l - abs(m));
		double down = (4 * MY_PI) * factorial(l + abs(m));

		return (double)sqrt(up/down);
	}

	//value for associated Legendre polynomial
	double static evaluateLegendre(double x, int l, int m)
	{
		double result = 0;
		if(l == m)
		{
			result = minusOnePower(m) * (double)doubleFactorial(2 * m - 1)*
					(double)pow(( 1 - x*x ),(double)m/2.0);
		}
		else if(l == m + 1)
		{
			result = x * (2 * m + 1) * evaluateLegendre(x,m,m);
		}
		else
		{
			result = (
				x *(2 * l - 1) * evaluateLegendre(x,l-1,m)
				- (l + m - 1) * evaluateLegendre(x,l-2,m) 
				)/(l - m);

		}
		return result;
	}
	
	//value for Spherical Harmonic
	double static SHvalue(double theta,double phi,int l, int m)
	{
		double result = 0;
		if(m == 0)
		{
			result = Kvalue(l,m) * evaluateLegendre(cos(theta),l,0);
		}
		else if(m > 0)
		{
			result = sqrt(2.0f) * Kvalue(l,m)* cos(m * phi) * evaluateLegendre(cos(theta),l,m);
		}
		else
		{
			result = sqrt(2.0f) * Kvalue(l,m) * sin(-m * phi) * evaluateLegendre(cos(theta),l,-m);
		}

		if(fabs(result) <= M_ZERO)
			result = 0.0;

		if(_isnan(result))
		{
			system("pause");
			std::cout << "SPHERICAL HARMONIC NAN" << std::endl;
			std::cout << "theta phi" << theta<<' ' << phi << std::endl;
		}

		return result;

	}


};


#endif
