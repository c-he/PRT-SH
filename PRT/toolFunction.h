#ifndef TOOLFUNCTION_H_
#define TOOLFUNCTION_H_


const double MY_PI = 3.14159265358979323846;
const double M_INFINITE = 1e10f;
const double M_ZERO = 0.000000001;
const float M_DELTA = 0.000001f;

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "GL/freeglut.h"
#include "glm\glm.hpp"
using glm::vec3;
using std::cout;
using std::endl;

struct Triangle
{
	vec3 _v0,_v1,_v2;
	int _index;
	Triangle()
	{
		_index = -1;
	}

	Triangle(vec3 v0,vec3 v1,vec3 v2,int index)
		:_v0(v0),_v1(v1),_v2(v2),_index(index){}

};


struct Ray
{
	vec3 _o;//origin
	vec3 _dir;//direction
	vec3 _inv;

	float _tmin;
	float _tmax;
	float _t;// intersect parameter
	int _index;//intersect index

	Ray()
	{}

	Ray(vec3 o,vec3 d,float tmin = 0.0f,float tmax = M_INFINITE)
		:_o(o),_dir(d),_tmin(tmin),_tmax(tmax)
	{
		normalize();
	}
	void normalize()
	{
		_inv.x = 1.0f/_dir.x;
		_inv.y = 1.0f/_dir.y;
		_inv.z = 1.0f/_dir.z;

		_t = (float)M_INFINITE;
		float length = sqrt(_dir.x * _dir.x + _dir.y * _dir.y + _dir.z * _dir.z);

		if(fabs(length) <= M_ZERO)
			return ;

		_dir.x /= length;
		_dir.y /= length;
		_dir.z /= length;
	}

};

inline float Trimax(float a,float b,float c)
{
	return std::max(std::max(a,b),std::max(a,c));
}

inline float Trimin(float a,float b,float c)
{
	return std::min(std::min(a,b),std::min(a,c));
}

inline float factorial(int n)
{
	if(n <= 1)
		return 1.0;

	float result  = 1.0;
	for(int i = 1; i <= n ; ++i)
	{
		result *= i;
	}

	if(_isnan(result))
	{
		std::cout << "Factorial Error." << std::endl;
	}

	return result;
}

inline double doubleFactorial(int n)
{
	if(n <= 1)
		return 1.0;

	double result = 1.0;
	for(unsigned i = n;i > 1; i -= 2)
	{
		result *= i;
	}

	
	if(_isnan(result))
	{
		std::cout << "Double Factorial Error." << std::endl;
	}

	return result;
}

inline int minusOnePower(int n)
{
	if(n % 2)
		return -1;
	else
		return 1;
}

inline float random0to1(int n)
{
	return (rand() % (n+1)) / (float)n;
}

inline void barycentric(vec3 pc,vec3 p[3],float &u,float &v,float &w)
{
	vec3 x = 100.0f *(p[1] - p[0]);
	vec3 y = 100.0f *(p[2] - p[0]);
	vec3 pv = 100.0f * (pc - p[0]);

	float A = glm::dot(x,x);
	float B = glm::dot(x,y);
	float C = glm::dot(y,y);
	float E = glm::dot(pv,x);
	float F = glm::dot(pv,y);

	float delta = A * C - B * B;

	if(fabs(delta) <= M_ZERO)
	{
		std::cout << "Three vertexes are colinear" << std::endl;

		for(int i = 0;i < 3; ++i)
			std::cout << i << ' ' << p[i].x << ' ' << p[i].y << ' ' << p[i].z <<std::endl;

		std::cout << "pc" << ' ' << pc.x << ' ' << pc.y << ' ' << pc.z <<std::endl;

		//system("pause");

		//exit(-1);
		u = v = w = 1/3.0f;
		return;
	}

	u = (E * C - B * F)/delta;
	v = (A * F - B * E)/delta;

	w = 1.0f - u - v;

	if(_isnan(u) || _isnan(v))
	{
		std::cout << "NaN!" << std::endl;
		exit(-1);
	}
}

inline bool rayTriangle(Ray &ray, Triangle& in)//vec3& p0,vec3 &p1,vec3 &p2)
{
	vec3 edge1 = in._v1 - in._v0;
	vec3 edge2 = in._v2 - in._v0;

	vec3 crossVec = glm::cross(ray._dir,edge2);

	float dotvalue = glm::dot(edge1,crossVec);

	if(fabs(dotvalue) < M_ZERO)
	{
		return false;
	}

	float inv = 1.0f/dotvalue;

	vec3 diffVec = ray._o - in._v0;


	float u = glm::dot(diffVec,crossVec) * inv ;
	if(u < 0.0f || u > 1.0f)
		return false;

	vec3 q = glm::cross(diffVec,edge1);

	float v = glm::dot(ray._dir,q) * inv;

	if(v < 0.0f || u  + v > 1.0)
		return false;

	float t = glm::dot(edge2,q) * inv;
	if(t <= 0.0f)
		return false;

	if(t < ray._t)
	{
		ray._t = t;
		ray._index = in._index;
	}

	return true;
}

inline GLubyte *loadBMP(std::string filename,int &width,int &height)
{
	GLint total_bytes;
	GLuint texture_ID = 0;
	GLubyte *pixels = NULL;
	FILE *pFile;

	fopen_s(&pFile,filename.c_str(),"rb");

	if(pFile == NULL)
	{
		std::cout<<"Image not found!"<<std::endl; 
		return NULL;
	}

	fseek(pFile,18,SEEK_SET);
	fread(&width,sizeof(width),1,pFile);
	fread(&height,sizeof(height),1,pFile);
	fseek(pFile,54,SEEK_SET);

	total_bytes = (width*3+(4-width*3%4)%4)*height;

	if((pixels=(GLubyte *)malloc(total_bytes))==NULL)
	{
		fclose(pFile);
		return NULL;
	}

	if(fread(pixels,total_bytes,1,pFile)<=0)
	{
		free(pixels);
		fclose(pFile);
		return NULL;
	}

	return pixels;
}
// return value [0,2 *M_PI]
inline float inverseSC(float sinV,float cosV)
{
	//sinV = glm::clamp(sinV,-1.0f,1.0f);
	cosV = glm::clamp(cosV,-1.0f,1.0f);
	float result = acos(cosV);
	if(sinV < 0)
		result = 2.0f * (float)MY_PI - result;
	return result;
}

inline void rotateMatrixtoZYZ(float mat[3][3], float &alpha,float& beta,float &gamma)
{
	beta = acos(glm::clamp(mat[2][2],-1.0f,1.0f));
	float sinbeta = sin(beta);
	if((fabs(mat[1][2]) + fabs(mat[0][2]) + fabs(mat[2][0])+fabs(mat[2][1])) < M_ZERO) 
	{
		if(mat[3][3] == -1)
		{
			
		}
		else
		{
			beta = 0.0f;
			gamma = 0.0f;
		}

		std::cout << "fuck zero" << std::endl;
		{
			std::cout << "the matrix"<<std::endl;
			for(int i = 0;i < 3; ++i)
				for(int j = 0;j < 3; ++j)
					std::cout << mat[i][j];

		}
	//	system("pause");
	}
	else
	{
		float cosalpha = mat[0][2]/sinbeta;
		float sinalpha = mat[1][2]/sinbeta;

		alpha = inverseSC(sinalpha,cosalpha);

		float cosgamma = -mat[2][0]/sinbeta;
		float singamma = mat[2][1]/sinbeta;

		gamma = inverseSC(singamma,cosgamma);

	}

}

inline void rotateMatrixtoYZY(float mat[3][3],float &alpha,float &beta,float &gamma)
{
/*	{
		//std::cout << "the matrix"<<std::endl;
		for(int i = 0;i < 3; ++i)
			for(int j = 0;j < 3; ++j)
				std::cout << mat[i][j];

	}*/
	beta = acos(glm::clamp(mat[1][1],-1.0f,1.0f));
	float sinbeta = sin(beta);
	if((fabs(mat[0][1]) + fabs(mat[1][0]) + fabs(mat[1][2])+fabs(mat[2][1])) < M_ZERO) 
	{
		if(mat[1][1] == -1)
		{
			beta = (float )MY_PI;
			gamma = 0.0f;
			alpha = atan2(mat[2][0],mat[0][0]);
			

			//system("pause");
		}
		else
		{
			//beta = M_PI
			//mat[1][1] == 1
			beta = 0.0f;
			gamma = 0.0f;
			//alpha = atan2(mat[0][2],mat[0][0]);
			alpha = inverseSC(mat[0][2],mat[0][0]);

		}

		
		//system("pause");
	}
	else
	{
		float cosalpha = -mat[0][1]/sinbeta;
		float sinalpha = mat[2][1]/sinbeta;

		alpha = inverseSC(sinalpha,cosalpha);

		float cosgamma = mat[1][0]/sinbeta;
		float singamma = mat[1][2]/sinbeta;

		gamma = inverseSC(singamma,cosgamma);

	}

}

inline void rotateMatrixtoXYX(float mat[3][3],float &alpha,float &beta,float &gamma)
{
	{
		//std::cout << "the matrix"<<std::endl;
		for(int i = 0;i < 3; ++i)
			for(int j = 0;j < 3; ++j)
				std::cout << mat[i][j];
	}
	beta = acos(glm::clamp(mat[0][0],-1.0f,1.0f));
	float sinbeta = sin(beta);
	if((fabs(mat[0][1]) + fabs(mat[0][2]) + fabs(mat[1][0])+fabs(mat[2][0])) < M_ZERO) 
	{
		if(mat[0][0] == -1)
		{
			beta = (float )MY_PI;
			gamma = 0.0f;
			alpha = atan2(mat[2][0],mat[0][0]);
			

		}
		else
		{
			//beta = M_PI
			//mat[1][1] == 1
			beta = 0.0f;
			gamma = 0.0f;
			alpha = atan2(mat[0][2],mat[0][0]);

		}

		
		//system("pause");
	}
	else
	{
		float cosalpha = -mat[0][2]/sinbeta;
		float sinalpha = mat[0][1]/sinbeta;

		alpha = inverseSC(sinalpha,cosalpha);

		float cosgamma = mat[0][2]/sinbeta;
		float singamma = mat[0][1]/sinbeta;

		gamma = inverseSC(singamma,cosgamma);

	}

}



#endif