#ifndef UIFUNCTION_H_
#define UIFUNCTION_H_

#include <cmath>
#include "GL/freeglut.h"


// axis,angle convert to quat
void AxisAngletoQuat(float *q, const float* axis, float angle)
{
	float sinxita ,length;

	sinxita = (float)sin(0.5f * angle);
	length = (float)sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);

	q[0] = sinxita * axis[0] / length;
	q[1] = sinxita * axis[1] / length;
	q[2] = sinxita * axis[2] / length;
	q[3] = (float)cos(0.5f * angle);

}

//quat to rotate matrix
void QuattoMatrix(const float *q ,float *mat)
{
	float yy2 = 2.0f * q[1] * q[1];
	float xy2 = 2.0f * q[0] * q[1];
	float xz2 = 2.0f * q[0] * q[2];
	float yz2 = 2.0f * q[1] * q[2];
	float zz2 = 2.0f * q[2] * q[2];
	float wz2 = 2.0f * q[3] * q[2];
	float wy2 = 2.0f * q[3] * q[1];
	float wx2 = 2.0f * q[3] * q[0];
	float xx2 = 2.0f * q[0] * q[0];
	mat[0] = - yy2 - zz2 + 1.0f;
	mat[1] = xy2 + wz2;
	mat[2] = xz2 - wy2;
	mat[4] = xy2 - wz2;
	mat[5] = - xx2 - zz2 + 1.0f;
	mat[6] = yz2 + wx2;
	mat[8] = xz2 + wy2;
	mat[9] = yz2 - wx2;
	mat[10] = - xx2 - yy2 + 1.0f;
	mat[3] = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0;
	mat[15] = 1;

}


// multiply quaternion
void Multi(const float *q1, const float *q2, float *result)
{
	result[0] = q1[3]*q2[0] + q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1];
	result[1] = q1[3]*q2[1] + q1[1]*q2[3] + q1[2]*q2[0] - q1[0]*q2[2];
	result[2] = q1[3]*q2[2] + q1[2]*q2[3] + q1[0]*q2[1] - q1[1]*q2[0];
	result[3]  = q1[3]*q2[3] - (q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2]);
}


int GetTimeMs()
{
	return glutGet(GLUT_ELAPSED_TIME);

}


#endif
