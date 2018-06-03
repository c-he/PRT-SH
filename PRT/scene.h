#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <string>

class Scene
{
public:
    std::vector<float> _vertexes;
    std::vector<float> _normals;
    std::vector<float> _texcoords;
	
private:
	float vmaxX,vmaxY,vmaxZ;
	float vminX,vminY,vminZ;

	std::string _fileName;
};

#endif
