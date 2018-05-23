#ifndef RENDERER_H_
#define RENDERER_H_

#include <vector>

#include "GL\glew.h"
#include "GL\freeglut.h"
#include "glm\glm.hpp"

#include "Scene.h"
#include "Object.h"
#include "Lighting.h"
#include "DiffuseObject.h"
#include "GeneralObject.h"

using std::vector;
using glm::vec3;


class Renderer
{
public:
	Renderer(){}
	void init(Scene *scene,Lighting *light)
	{
		_scene = scene;
		_lighting = light;
	}
	void init(DiffuseObject *obj,Lighting *light)
	{
		_diffObject = obj;
		_lighting = light;
		_genObject = NULL;
	}

	void init(GeneralObject *obj,Lighting *light)
	{
		_genObject = obj;
		_lighting = light;
		_diffObject = NULL;
	}

	void toGPUObject();
	void naiveObjRender();

	void initColorBuffer(int type,vec3 viewDir,bool diffornot);
	void initDiffuseBuffer(int type);
	void initGeneralBuffer(int type,vec3 viewDir);

	

private:
	Scene *_scene;
	DiffuseObject *_diffObject;
	GeneralObject *_genObject; 

	
	Lighting *_lighting;
	unsigned _vao;
	unsigned _vboVertex,_vboNormal,_vboColor;

	vector<float> _colorBuffer;
};





#endif