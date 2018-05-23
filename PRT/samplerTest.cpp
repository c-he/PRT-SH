#include <stdlib.h>
#include <iostream>
#include <cmath>

//#include <AntTweakBar.h>
#include <GL/glut.h>
#include <glm/glm.hpp>

#include "Sampler.h"
#include "UIFunction.h"
#include "toolFunction.h"
#include "SphericalHarmonic.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

Sampler stemp(64);
int l = 1,m = -1;

float g_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
int g_AutoRotate = 0;
int g_RotateTime = 0;
float g_RotateStart[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float rotateMatrix[4*4]; // rotation matrix


void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();          
   gluLookAt (0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   glBegin(GL_LINES);
	//	glColor3f(1.0f,0.0f,0.0f);
		glColor3i(50.0f,50.0f,50.0f);

		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(10.0f,0.0f,0.0f);
  
		//glColor3f(0.0f,1.0f,0.0f);
		glColor3i(200.0f,200.0f,200.0f);

		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,10.0f,0.0f);
  

		//glColor3f(0.0f,0.0f,1.0f);
		glColor3i(255.0f,255.0f,255.0f);

		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,10.0f);
		
   glEnd();


   glBegin(GL_POINTS);
   for(int i = 0; i < stemp._samples.size() ;++i)
   {
	   Sample temp = stemp._samples[i];
	   int index = l * (l + 1) + m;
	   if(temp._SHvalue[index] < 0)
	   {
		   glColor3f(0.0,0.0,1.0f);
	   }
	   else
	   {
		   glColor3f(1.0f,0.0f,0.0f);
	   }
	   vec3 pos = fabs(temp._SHvalue[index]) * temp._cartesCoord;
	   glPointSize(3.0f);
	   glVertex3f(pos.x,pos.y,pos.z);

   }
   glEnd();


    glutSwapBuffers();

}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glOrtho(-1.0, 1.0, -1.0, 1.0, 0.3, 20.0);
   glMatrixMode (GL_MODELVIEW);

}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }

   if(key == 'l')
   {
	   if(l < 4)
		l++;
   }
   if(key == 'k')
   {
	   if(l > 0)
	   l--;
   }
   if(key == 'm')
   {
	   if(m > -l || m < l)
		   m++;
   }
   if(key == 'n')
   {
	   if(m > -l|| m < l )
		   m--;
   }
   if(key == 'r')
   {
		l = m = 0;
   }

   std::cout << "l:" << l << std::endl;
   std::cout << "m:" << m << std::endl;


   glutPostRedisplay();

}

float f(vec3 in)
{
	if(l == 0 && m ==0)
	{
		return 1.0f/(2 * sqrt(M_PI));
	}
	if(l == 1 && m == -1)
	{
		return -(sqrt(3) * in.y)/(2 * sqrt(M_PI));
	}
	/*if(l == 1 && m == -1)
	{
		return -(sqrt(3) * in.x)/(2 * sqrt(M_PI));
	}*/

}

void check()
{
	int index  = l *(l + 1) +m;

	for(int i = 0;i < stemp._samples.size(); ++i)
	{
		double diff = fabs(stemp._samples[i]._SHvalue[index] - f(stemp._samples[i]._cartesCoord));
		if(diff > 0.0000001)
		{
			Sample temp = stemp._samples[i];
			system("pause");
			std::cout << diff << std::endl;
			
			std::cout << "SH error" << std::endl;
		}
	}

}



int main3(int argc, char** argv)
{
	stemp.computeSH(5);
	

	//(x,y,z) = (0,0,1)
/*	std::cout << SphericalH::SHvalue(0,0,0,0) << std::endl;
	std::cout << SphericalH::SHvalue(0,0,1,-1) << std::endl;
	std::cout << SphericalH::SHvalue(0,0,1,0) << std::endl;
	std::cout << SphericalH::SHvalue(0,0,1,1) << std::endl;

	std::cout << SphericalH::SHvalue(0,0,2,-2) << std::endl;
	std::cout << SphericalH::SHvalue(0,0,2,-1) << std::endl;
	std::cout << SphericalH::SHvalue(0,0,2,0) << std::endl;
	std::cout << SphericalH::SHvalue(0,0,2,1) << std::endl;
	std::cout << SphericalH::SHvalue(0,0,2,2) << std::endl;*/

	std::cout << SphericalH::SHvalue(M_PI/2.0,0,0,0) << std::endl;
	std::cout << SphericalH::SHvalue(M_PI/2.0,0,1,-1) << std::endl;
	std::cout << SphericalH::SHvalue(M_PI/2.0,0,1,0) << std::endl;
	std::cout << SphericalH::SHvalue(M_PI/2.0,0,1,1) << std::endl;


	
	





	//std::cout << cos(acos(1.0f)) << std::endl;





	check();

/*	for(int i = 0;i < 100; ++i)
	{
		float rtemp = random0to1(4096);
		if(rtemp < 0 || rtemp > 1.0f)
		{
			system("pause");
		
			std::cout << "ramdom problem" << std::endl;
		}
		std::cout << random0to1(4096) << endl;
	}*/


   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();

   
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
 

   glutMainLoop();
   return 0;
}


