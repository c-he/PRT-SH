#include <ctime>
#include <string>
#include <iostream>
#include <Windows.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include "AntTweakBar.h"
#define GLEW_STATIC
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
//#include <GL/glm/gtc/matrix_transform.hpp>

#include "UI.h"
#include "Scene.h"
#include "Object.h"
#include "Cubemap.h"
#include "Lighting.h"
#include "Renderer.h"
#include "UIFunction.h"
#include "GlutFunction.h"
#include "DiffuseObject.h"
#include "GeneralObject.h"

/*some process commends

prt -l xxxx.jpg output.txt bandnumber samplenumber(4096)
prt -o -d 1 xxxx.obj output.txt bandnumber samplenumber
prt -o -g 1 xxxx.obj output.txt bandnumber samplenumber

Todo: binary IO

*/

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;

int lightNumber = 3;
int ObjectNumber = 2;
int GeneralNumber = 2;

string objects[] = {"buddha","maxplanck","Gargoyle2"};
string gobjects[] = {"buddha","maxplanck","Gargoyle2"};
string lightings[] = {"grace","stpeters","campus"};
string transferF[] = {"D","DS","DSI"};
string albedos[] = {"01","03","05"};

vec3 HDRaffect[] = {vec3(2.2f,2.2f,2.2f),vec3(1.8f,1.8f,1.8f),vec3(0.5f,0.55f,0.5f)};
vec3 Glossyaffect[] = {vec3(1.2f,1.2f,1.2f),vec3(1.5f,1.5f,1.5f),vec3(0.3f,0.32f,0.3f)};

int objectIndex = -1;
int lightingIndex = -1;
int transferFIndex = -1;
int materialIndex = 0;
int albedosIndex = -1;

int lastMaterial = 0;
int lastObject = -1;
int lastLighting = -1;
int lastTransfer = -1;

DiffuseObject *diffObject;
GeneralObject *genObject;
Cubemap *cubeMap;
Lighting *lighting;
Lighting simpleL;
Renderer renderer;

vec3 albedo(0.15f,0.15f,0.15f);
////////////////////////
bool b_multiSampling = false;
bool b_lastmulti = false;
//////////////////cubemap
bool drawCubemap = true;
bool simpleLight = false;
bool lastSimple = false;
mat4 proMatrix;
/////////////////camera 
float camera_dis = 3;
float last_camera_pos[] = {0,0,1};
float camera_pos[] = {0,0,1};
float camera_dir[] = {0,0,0};
float camera_up[] = {0,1,0};
/////////////////simple lighting
float light_dir[] = {0,0,1};
float simpleLightTheta = 43.3f;
float simpleLightPhi = 225.0f;
////////////////mouse
int mouseX,mouseY;
int lastMouseX,lastMouseY;
////////////////rotation
float g_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
int g_AutoRotate = 0;
int g_RotateTime = 0;
float g_RotateStart[] = { 0.0f, 0.0f, 0.0f, 1.0f };

float g_MatDiffuse[] = { 1.0f, 1.0f, 0.0f, 1.0f };
float lastQuat[4];

float rotateMatrix[4*4]; // rotation matrix

bool b_rotate = false;
//////////////////info
/////fps
DWORD currTime,lastTime;
int frames,fps;
/////mesh
int vertexs;
int faces;

void calculateFPS()
{
	currTime = timeGetTime();
	++frames;

	if((currTime - lastTime) >= 1000.0f)
	{
		fps = (int)((float)frames * 1000.0f)/(currTime - lastTime);
		frames = 0;
		lastTime = currTime;
	}

}

void process(int n, char** var)
{
	string ptype(var[1]);
	string diffGeneal(var[2]);

	int samplenumber = 4096;
	int band = 4;

	int type = atoi(var[3]);
	
	if(n > 6)
	{
		band = atoi(var[6]);
	}
	if(n > 7)
	{
		samplenumber = atoi(var[7]);	
	}
	if(ptype == "-l")
	{

		Lighting ltemp(var[2],PROBE,band);
		//Lighting ltemp;
		ltemp.process(samplenumber,true);
		ltemp.write2Disk(var[3]);

	}
	else if(ptype == "-o")
	{
		double start,end;
		start = GetTickCount();
		if(diffGeneal == "-d")
		{
			DiffuseObject otemp;
			otemp.init(var[4],albedo);
			otemp.project2SH(type,band,samplenumber,2);
			otemp.write2Diskbin(var[5]);
		}
		else
		{
			GeneralObject otemp;
			otemp.init(var[4],albedo);
			otemp.project2SH(type,band,samplenumber,1);
			otemp.write2Disk(var[5]);
		}
		end  = GetTickCount();

		std::cout << "Training time:" << (end - start)/1000.0f << std::endl;
	}
}

void Display()
{
	b_rotate = false;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(drawCubemap)
	{
		glm::mat4 pvm =	proMatrix * 
		glm::lookAt(
			vec3(camera_dis * camera_pos[0],
				camera_dis * camera_pos[1],
				camera_dis * camera_pos[2]),
			vec3(camera_dir[0],camera_dir[1],camera_dir[2]),
			vec3(camera_up[0],camera_up[1],camera_up[2])
		);

		cubeMap[lightingIndex].render(pvm);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		camera_dis * camera_pos[0],
		camera_dis * camera_pos[1],
		camera_dis * camera_pos[2],
		camera_dir[0],camera_dir[1],camera_dir[2],
		camera_up[0],camera_up[1],camera_up[2]
	);	



	//std::cout << camera_pos[0] << ' ' << camera_pos[1] << ' ' << camera_pos[2] <<std::endl;

	if( g_AutoRotate ) 
	{
		float axis[3] = { 0, 1, 0 };
		float angle = (GetTimeMs()-g_RotateTime)/1000.0f;
		float quat[4];
		AxisAngletoQuat(quat, axis, angle);
		Multi(g_RotateStart, quat, g_Rotation);
		b_rotate = true;
	}

	QuattoMatrix(g_Rotation,rotateMatrix);
	glMultMatrixf(rotateMatrix);

	//for(int s = 0; s < 4; ++s)
		//std::cout << "quan" << g_Rotation[0] << ' '<< g_Rotation[1] << ' '<< g_Rotation[2] << ' '<< g_Rotation[3] << std::endl;
	vec3 rotateVector;
	bool b_rotateLight = false;
	float thetatemp;
	//float phitemp;
	if(b_rotate)
	{
		mat4 rM = glm::make_mat4(rotateMatrix);
		vec4 dir = rM * vec4(0.0f,0.0f,1.0f,0.0f);
		rotateVector = vec3(dir.x,dir.y,dir.z);
		rotateVector = glm::clamp(rotateVector,-1.0f,1.0f);
		thetatemp = acos(rotateVector.z);
		if(dir.x < 0)
			thetatemp = 2*(float)M_PI - thetatemp;

		b_rotateLight = true;
	}
	if(simpleLight)
	{
		//rotateVector = vec3(light_dir[0],light_dir[1],light_dir[2]);
		rotateVector = vec3(light_dir[2],light_dir[0],light_dir[1]);

		b_rotateLight = true;
	}

	if(b_rotateLight)
	{
		rotateVector = glm::normalize(rotateVector);
		float theta,phi;

		rotateVector[2] = glm::clamp(rotateVector[2],-1.0f,1.0f);

		theta = acos(rotateVector[2]);
		float sintheta = sin(theta);
		if(fabs(sintheta) < M_ZERO)
		{
			phi = 0.0f;
		}
		else
		{ 
			float cosphi = rotateVector[0]/sintheta;
			float sinphi = rotateVector[1]/sintheta;
			phi = inverseSC(sinphi,cosphi);
		}
		vector<vec2> rotatePara;
		rotatePara.clear();

		if(simpleLight)
		{
			rotatePara.push_back(vec2(theta,phi));
			simpleL.rotateZYZ(rotatePara);
		}
		if(b_rotate)
		{
			
			rotatePara.push_back(vec2(0.0f,2.0f*M_PI-thetatemp));
			//rotatePara.push_back(vec2(3.0f*M_PI/2.0f,M_PI/2.0f));
			lighting[lightingIndex].rotateZYZ(rotatePara);
		}
		if(materialIndex == 0)
		{
			renderer.init(&diffObject[objectIndex],&lighting[lightingIndex]);
			renderer.initColorBuffer(transferFIndex,vec3(0.0f,0.0f,0.0f),true);
		}
		else
		{
			renderer.init(&genObject[objectIndex],&lighting[lightingIndex]);
			renderer.initColorBuffer(transferFIndex, camera_dis * vec3(camera_pos[0],camera_pos[1],camera_pos[2]),false);
		}

	}

	if(materialIndex == 1)
	{
		if((last_camera_pos[0] != camera_pos[0])||(last_camera_pos[1] != camera_pos[1])||(last_camera_pos[2]!=camera_pos[2]))
		{
			renderer.init(&genObject[objectIndex],&lighting[lightingIndex]);
			renderer.initColorBuffer(transferFIndex,camera_dis * vec3(camera_pos[0],camera_pos[1],camera_pos[2]),false);
			last_camera_pos[0] = camera_pos[0];
			last_camera_pos[1] = camera_pos[1];
			last_camera_pos[2] = camera_pos[2];

		}

	}

	renderer.naiveObjRender();

	calculateFPS();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	TwDraw();
	glutSwapBuffers();
	glutPostRedisplay();

}

void synchronize()
{
	lastLighting = lightingIndex;
	lastObject = objectIndex;
	lastTransfer = transferFIndex;

	b_lastmulti = b_multiSampling;

	lastMouseX = mouseX;
	lastMouseY = mouseY;
}

void dataInit()
{
	std::cout << "Loading data....please wait.\n";
	
	diffObject = new DiffuseObject[ObjectNumber];
	genObject = new GeneralObject[GeneralNumber];
	
	lighting = new Lighting[lightNumber];
	cubeMap = new Cubemap[lightNumber];

	for(int i = 0;i < ObjectNumber;++i)
	{
		string objectString = "Scene/" + objects[i] + ".obj";
		diffObject[i].init(objectString,albedo);

		objectString = "ProcessedData/" + objects[i] ;
		
		diffObject[i].readFDiskbin(objectString);
	}

	// for(int i = 0;i < GeneralNumber; ++i)
	// {
	// 	string objectString = "Scene/" + gobjects[i] + ".obj";
	// 	genObject[i].init(objectString,albedo);
 //
	// 	objectString = "ProcessedData/" + gobjects[i] ;
	// 	
	// 	genObject[i].readFDisk(objectString);
	// }
	 
	for(int i = 0;i < lightNumber; ++i)
	{
		string lightString = "ProcessedData/"+lightings[i] + "_probe.dat";
		lighting[i].init(lightString,HDRaffect[i],Glossyaffect[i]);

	}

	std::cout <<"Init data." << std::endl;

	albedosIndex = 0;
	transferFIndex = 0;
	//changeScene(1,0);
//	changeLight(0);
//	changeObject(1);

	objectIndex = 0;
	lightingIndex = 0;


	simpleL.init("ProcessedData/simple_probe.dat",vec3(1.0f,1.0f,1.0f),vec3(1.0f,1.0f,1.0f));//light data first then renderer

	if(materialIndex == 0)
	{
		//Diffuse Rendering
		renderer.init(&diffObject[0],&lighting[0]);
		renderer.initColorBuffer(0,vec3(0.0f,0.0f,1.0f),true);
	}
	else
	{
		// General Rendering
		renderer.init(&genObject[0],&lighting[0]);
		renderer.initColorBuffer(0,camera_dis * vec3(camera_pos[0],camera_pos[1],camera_pos[2]),false);
	}

	synchronize();
}

int main(int argc, char** argv)
{
	//preprocess light
	/*argc = 6;
	argv[0] = "prt";
	argv[1] = "-l";
	
	argv[2] = "Lighting/cubemaptest.bmp";
	argv[3] = "ProcessedData/simple_probe.dat";

	argv[4] = "4";
	argv[5] = "4096";*/

	//preprocess model
	/*argc = 7;
	argv[0] = "prt";
	argv[1] = "-o";
	argv[2] = "-g";
	argv[3] = "1";
	argv[4] = "Scene/buddha.obj";
	argv[5] = "Time/buddhaG01.dat";
	argv[6] = "4";
	argv[7] = "4096";*/
	

	if(argc >= 2)
	{
		process(argc,argv);
	}
	else
	{
		dataInit();

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH| GLUT_DOUBLE|GLUT_MULTISAMPLE);


		glutInitWindowSize(800, 800);
		glutCreateWindow("Nichaohao");
		glutCreateMenu(NULL);

		glutDisplayFunc(Display);
		glutReshapeFunc(Reshape);
		//glutMouseFunc(Mouse);
		glutIdleFunc(Idle);

		glewInit();

		if (!glewIsSupported("GL_VERSION_2_0"))
		{
			printf("OpenGL 2.0 not supported\n");
			exit(1);
		}

		for(int i = 0;i < lightNumber;++i)
		{
			cubeMap[i].init("LightingCube/" + lightings[i]);
			cubeMap[i].setup();
		}
		
		glInit();
		UIInit();

		//prepare the time
		lastTime = timeGetTime();
	    glutMainLoop();
	}
	

    return 0; 

}


