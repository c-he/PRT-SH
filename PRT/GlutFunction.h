#ifndef GLUTFUNCTION_H_
#define GLUTFUNCTION_H_

#include "AntTweakBar.h"

#include "GL/freeglut.h"


#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "Object.h"
#include "Lighting.h"
#include "Renderer.h"
#include "DiffuseObject.h"
#include "GeneralObject.h"

using glm::mat4;

extern float camera_dis;
extern mat4 proMatrix;

extern int objectIndex;
extern int lightingIndex;
extern int transferFIndex;
extern int albedosIndex ;

extern int mouseX;
extern int mouseY;
extern int lastMouseX;
extern int lastMouseY;

extern int lastMaterial;
extern int materialIndex;
extern int lastObject;
extern int lastLighting;
extern int lastTransfer;
extern bool lastSimple;
extern bool simpleLight;
extern bool b_lastmulti;
extern bool b_multiSampling;
extern bool drawCubemap;
extern float simpleLightTheta;
extern float simpleLightPhi;
extern int ObjectNumber;
extern int GeneralNumber;

extern DiffuseObject *diffObject;
extern GeneralObject *genObject;
extern Lighting *lighting;
extern Renderer renderer;

extern vec3 albedo;
extern int vertexs;
extern int faces;

extern float camera_dis;
extern float camera_pos[];


void changeLight(int index)
{
//	string lightString = "ProcessedData/"+lightings[index] + "_probe.txt";
//	lighting.init(lightString);
	if(materialIndex == 1)
	{
		renderer.init(&genObject[objectIndex],&lighting[index]);
		renderer.initColorBuffer(transferFIndex,camera_dis *vec3(camera_pos[0],camera_pos[1],camera_pos[2]),false);
	}
	else
	{
		renderer.init(&diffObject[objectIndex],&lighting[index]);
		renderer.initColorBuffer(transferFIndex,vec3(0.0f,0.0f,0.0f),true);
	}
}

void changeObject(int index)
{

	if(materialIndex == 0)
	{
		renderer.init(&diffObject[index],&lighting[lightingIndex]);
		renderer.initColorBuffer(transferFIndex,vec3(0.0f,0.0f,0.0f),true);
	}
	else
	{
		renderer.init(&genObject[index],&lighting[lightingIndex]);
		renderer.initColorBuffer(transferFIndex,camera_dis *vec3(camera_pos[0],camera_pos[1],camera_pos[2]),false);
	}
}

void changeTransfer(int index)
{
	if(materialIndex == 0)
	{
		renderer.init(&diffObject[objectIndex],&lighting[lightingIndex]);
		renderer.initColorBuffer(index,vec3(0.0f,0.0f,0.0f),true);
	}
	else
	{
		renderer.init(&genObject[objectIndex],&lighting[lightingIndex]);
		renderer.initColorBuffer(transferFIndex,camera_dis *vec3(camera_pos[0],camera_pos[1],camera_pos[2]),false);
	}
}

void glInit()
{
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glShadeModel(GL_SMOOTH);
}

void Idle()
{
	//std::cout << "idle" << std::endl;
	bool changed = false;
	if(lastLighting != lightingIndex)
	{
		std::cout << "Light change" << std::endl;

		changeLight(lightingIndex);
		lastLighting = lightingIndex;
		changed = true;
	}
	if(lastObject != objectIndex)
	{
		std::cout << "Object change" << std::endl;
		changeObject(objectIndex);
		lastObject = objectIndex;
		changed = true;

	}
	if(lastTransfer != transferFIndex)
	{
		if((materialIndex == 0) || ((materialIndex == 1)&&(objectIndex == 0)))
		{
			changeTransfer(transferFIndex);
			lastTransfer = transferFIndex;

			changed = true;
		}

	
	}
	if(lastSimple != simpleLight)
	{
		changed = true;
		renderer.initColorBuffer(transferFIndex,vec3(0.0f,0.0f,0.0f),true);
		std::cout << "Simple " << changed << std::endl;
		lastSimple = simpleLight;
		if(simpleLight)
			drawCubemap = false;
		else
			drawCubemap = true;

	}
	if(lastMaterial != materialIndex)
	{
		if(((materialIndex == 0) && (objectIndex < ObjectNumber)) || ((materialIndex == 1)&&(objectIndex < GeneralNumber)))
		{
			changeLight(lightingIndex);
			changed = true;
		}
		std::cout << materialIndex << std::endl;

		lastMaterial = materialIndex;
	}
	if(b_lastmulti != b_multiSampling)
	{
		if(b_multiSampling)
		{
			glEnable(GL_MULTISAMPLE);
		}
		else
		{
			glDisable(GL_MULTISAMPLE);
		}
	}

	if(changed)
	{
		glutPostRedisplay();
	}

}

int MouseMove(int x, int y)
{
	
	if(simpleLight)
	{
			mouseX = x;
			mouseY = y;

			simpleLightTheta += 0.1f*(mouseY - lastMouseY);
			if(simpleLightTheta < 0.0f)
				simpleLightTheta = 0.0f;
			if(simpleLightTheta > 180.0f)
				simpleLightTheta = 180.0f;

			simpleLightPhi += 0.1f * (mouseX - lastMouseX);

			lastMouseX = mouseX;
			lastMouseY = mouseY;

	}

	return TwEventMouseMotionGLUT(x,y);
}


int Mouse(int button,int state, int x, int y)
{
	/*if(button == GLUT_LEFT && state == GLUT_DOWN && x >= 0 && x<= ww && y >=0 &&y <= wh)
	{

	}*/


	if(button == 3)
	{
		if(state == GLUT_UP)
			return 0;
		 camera_dis -= 0.2f;
	}
	else if(button == 4)
	{
		if(state == GLUT_UP)
			return 0;
		 camera_dis += 0.2f;

	}
	else
	{
		//printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
	}
	return TwEventMouseButtonGLUT(button, state,x,y);
}


int Keyboard(unsigned char key, int x, int y)
{
	//key_buffer[key] = true;

	if(key == 27)
	{
		exit(0);
	}

	return TwEventKeyboardGLUT(key,x,y);

}

void Reshape(int width, int height) 
{
	glMatrixMode(GL_VIEWPORT);
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));              
	glMatrixMode(GL_PROJECTION);                                     
	glLoadIdentity();                                                  
	gluPerspective(45, (float)width/(float)height, 0.1, 1000);          
	glMatrixMode(GL_MODELVIEW);                                       
	glLoadIdentity();

//	float test[16];
//	glGetFloatv(GL_PROJECTION_MATRIX,test);

	proMatrix = glm::perspective(45.0f, (float)width/(float)height, 0.1f, 10000.0f);
}


#endif