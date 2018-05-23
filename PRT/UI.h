#ifndef UI_H_
#define UI_H_

#include "AntTweakBar.h"

#include "GL/freeglut.h"

#include "Object.h"
#include "UIFunction.h"
#include "GlutFunction.h"

extern float g_Rotation[4];
extern float camera_pos[];
extern float light_dir[];
extern int g_AutoRotate ;
extern int g_RotateTime ;
extern float g_RotateStart[4];

extern int fps;
extern int vertexs;
extern int faces;
extern string objects[];
extern string lightings[];
extern int objectIndex;
extern int lightingIndex;
extern int transferFIndex;
extern int materialIndex;


extern bool simpleLight;
extern bool b_multiSampling;
//extern bool b_objectMode;


typedef enum {UNSHADOW, SHADOW, INTERREFLECT } TransferFEUM;
typedef enum {BUDDHA,MAXPLANCK} ObjectEUM;
typedef enum {LIGHT1,LIGHT2,LIGHT3} LightingEUM;
typedef enum {DIFFUSE,GLOSSY} MaterialEUM;


// Function called at exit
void Terminate(void)
{
	TwTerminate();
}

void TW_CALL SetAutoRotateCB(const void *value, void *clientData)
{
	(void)clientData; // unused

	g_AutoRotate = *(const int *)value; // copy value to g_AutoRotate
	if( g_AutoRotate!=0 ) 
	{
		// init rotation
		g_RotateTime = GetTimeMs();
		g_RotateStart[0] = g_Rotation[0];
		g_RotateStart[1] = g_Rotation[1];
		g_RotateStart[2] = g_Rotation[2];
		g_RotateStart[3] = g_Rotation[3];

		TwDefine(" TweakBar/ObjRotation readonly ");
	}
	else
		TwDefine(" TweakBar/ObjRotation readwrite ");
}


void TW_CALL GetAutoRotateCB(void *value, void *clientData)
{
	(void)clientData; // unused
	*(int *)value = g_AutoRotate; // copy g_AutoRotate to value
}



void UIInit()
{
	TwBar *bar,*info; 
	float axis[] = { 0.0f, 1.0f, 0.0f }; 
	float angle = 0.0f;

	TwInit(TW_OPENGL, NULL);

	
	glutMouseFunc((GLUTmousebuttonfun)Mouse);
	glutMotionFunc((GLUTmousemotionfun)MouseMove);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutKeyboardFunc((GLUTkeyboardfun)Keyboard);
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	TwGLUTModifiersFunc(glutGetModifiers);
	//TwCopyStdStringToClientFunc(CopyStdStringToClient); 

	bar = TwNewBar("TweakBar");
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLUT and OpenGL.' "); 
	TwDefine(" TweakBar size='250 440' color='96 216 224' position='3 10' "); 

	//TwAddVarRW(bar, "Material Type", TW_TYPE_BOOL32,&b_objectMode,NULL);	
	{
		TwEnumVal MaterialEV[2] = {{DIFFUSE,"diffuse"},{GLOSSY, "glossy"}};
		TwType MaterialType = TwDefineEnum("Material Type", MaterialEV, 2); 
		TwAddVarRW(bar, "Material Type", MaterialType, &materialIndex, " keyIncr='<' keyDecr='>' help='Change object.' ");
	}
	
	TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation, 
		" label='Object rotation' opened=false help='Change the object orientation.' ");

	TwAddVarCB(bar, "AutoRotate", TW_TYPE_BOOL32, SetAutoRotateCB, GetAutoRotateCB, NULL, 
		" label='Auto-rotate' key=space help='Toggle auto-rotate mode.' ");	

	TwAddVarRW(bar,"CameraRotation",TW_TYPE_DIR3F,&camera_pos,
		" label='Camera rotation' opened=true help='Change the camera direction.'");

	TwAddVarRW(bar,"Simple Light", TW_TYPE_BOOLCPP,&simpleLight,
		" label='Simple Light' help='Change the simple light'");


	TwAddVarRW(bar,"LightRotation",TW_TYPE_DIR3F,&light_dir,
		" label='Light rotation' opened=true help='Change the camera direction.'");

	{
		TwEnumVal ObjectEV[2] = {{BUDDHA,"buddha"},{MAXPLANCK, "maxplanck"}};
		TwType ObjectType = TwDefineEnum("ObjectType", ObjectEV, 2); 
		TwAddVarRW(bar, "Object Type", ObjectType, &objectIndex, " keyIncr='<' keyDecr='>' help='Change object.' ");
	}

	{
		TwEnumVal tranFEV[3] = { {UNSHADOW, "Unshadowed"}, {SHADOW, "Shadowed"}, {INTERREFLECT, "Interreflect"} };
		TwType tranFType = TwDefineEnum("tranFType", tranFEV, 3);
		TwAddVarRW(bar, "Transfer Type", tranFType, &transferFIndex, " keyIncr='<' keyDecr='>' help='Change transfer function.' ");
	}

	{
		TwEnumVal LightingEV[3] = {{LIGHT1,"Light1"},{LIGHT2,"Light2"},{LIGHT3,"Light3"}};
		TwType LightType = TwDefineEnum("Lighting",LightingEV,3);
		TwAddVarRW(bar,"Lighting",LightType,&lightingIndex," keyIncr='<' keyDecr='>' help='Change object.' ");
	}
	TwAddVarRW(bar, "Multi Sampling", TW_TYPE_BOOL32,&b_multiSampling,NULL);	
	
	info = TwNewBar("Information");
	TwDefine(" Information size='200 320' text=light  color='40 40 40' position='3 450' valueswidth=100"); 

	TwAddVarRO(info, "FPS", TW_TYPE_UINT32, &fps, " label='FPS :' ");

	string obj = objects[objectIndex];
	string l = lightings[lightingIndex];


	//TwAddVarRO(info,"Mesh",TW_TYPE_STDSTRING,&obj," label='Mesh:' ");
	//TwAddVarRO(info,"Lighting",TW_TYPE_STDSTRING,&l," label='Lighting:' ");
	TwAddVarRO(info, "nbV", TW_TYPE_UINT32, &vertexs, " label='Vertices :' ");
	TwAddVarRO(info, "nbF", TW_TYPE_UINT32, &faces, " label='Triangles :' ");






		
	g_RotateTime = GetTimeMs();

	AxisAngletoQuat(g_Rotation,axis,angle);
	AxisAngletoQuat(g_RotateStart,axis,angle);

}


#endif
