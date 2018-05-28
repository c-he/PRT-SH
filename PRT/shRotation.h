#ifndef SHROTATION_H_
#define SHROTATION_H_

#include <cmath>
#include <vector>
#include <Eigen/Dense>

using std::vector;
using Eigen::MatrixXf;
using Eigen::VectorXf;

const float sqrt3chu2 = 0.8660254037844f;
const float sqrt625 = 0.790569415042f;
const float sqrt375 = 0.612372435696f;
const float sqrt15chu4 = 0.96824583655f;

inline void singleZMatrix(int band, float angle, MatrixXf &res)
{
	int size = 2 * band + 1;

	for(int i = 0;i < size; ++i)
	{
		for(int j = 0;j < size; ++j)
		{
			res(i,j) = 0.0f;

			if((i == band) && (j == band))
			{
				res(i,j) = 1.0f;
			}
			if(i < band)
			{
				int k = band - i;

				if(j == i)
					res(i,j) = cos(k * angle);
				if(j == (size - i - 1))
					res(i,j) = sin(k * angle);

			}
			if(i > band)
			{
				int k = i - band;

				if(j == i)
					res(i,j) = cos(k * angle);
				if(j == (size - i -1))
					res(i,j) = -sin(k * angle);

			}

		}
	}
}


inline void ZRotateMatrix(int band,float angle, vector<MatrixXf> &res)
{
	for(int i = 0;i < band; ++i)
	{
		int size = 2 * i + 1;
		MatrixXf mtemp(size,size);

		singleZMatrix(i,angle,mtemp);
		res.push_back(mtemp);
	}
}

inline void singleXMatrix(int band, MatrixXf &res)
{
	res.setZero();
	if(band == 0)
	{
		res(0,0) = 1.0f;
	}
	if(band == 1)
	{
		res(0,1) = 1.0f;
		res(1,0) = -1.0f;
		res(2,2) = 1.0f;
	}
	if(band == 2)
	{
		res(0,3) = 1.0;
		res(1,1) = -1.0;
		res(2,2) = -0.5;
		res(2,4) = -sqrt3chu2;
		res(3,0) = -1.0;
		res(4,2) = -sqrt3chu2;
		res(4,4) = 0.5;
	}
	if(band == 3)
	{
		res(0,3) = -sqrt625;
		res(0,5) = sqrt375;
		
		res(1,1) = -1.0;
		
		res(2,3) = -sqrt375;
		res(2,5) = -sqrt625;
		
		res(3,0) = sqrt625;
		res(3,2) = sqrt375;
		
		res(4,4) =-0.25;
		res(4,6) =-sqrt15chu4;
		
		res(5,0) =-sqrt375;
		res(5,2) = sqrt625;

		res(6,4) =-sqrt15chu4;
		res(6,6) = 0.25;
	}


}

inline void XRotateMatrix(int band, vector<MatrixXf> &res)
{
	for(int i = 0;i < band; ++i)
	{
		int size = 2 * i + 1;
		MatrixXf mtemp(size,size);

		singleXMatrix(i,mtemp);
		res.push_back(mtemp);
	}

}

inline void transfer(int band, vector<MatrixXf> &m, VectorXf &v, bool transport = false)
{
	for(int i = 0;i < band; ++i)
	{
		int offset = i * i ;
		float *result = new float[2 * i + 1];
		int size = 2 * i + 1;

		for(int j = 0;j < size; ++j)
		{
			result[j] = 0.0f;
			if(transport)
			{
				for(int k = 0; k < size; ++k)
					result[j] += m[i](k,j) * v(offset + k);
			}
			else
			{
				for(int k = 0; k < size; ++k)
					result[j] += m[i](j,k) * v(offset + k);
			}

		}
		
		for(int j = 0; j < size; ++j)
			v(offset + j) = result[j];

		delete [] result;
	}

}


#endif