#ifndef SCENE_H_
#define SCENE_H_

#include <iostream>
#include <vector>

#include "toolFunction.h"

using std::string;
using std::vector;


class Scene
{
public:
	

private:
	float vmaxX,vmaxY,vmaxZ;
	float vminX,vminY,vminZ;

	string _fileName;
public:
	vector<float> _vertexes;
	vector<float> _normals;
	vector<float> _texcoords;

};



#endif