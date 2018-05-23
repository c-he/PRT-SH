#include <iostream>
#include "BBox.h"

using namespace std;

int main3()
{


	BBox temp;
	temp._v[0] = vec3(1,1,1);
	temp._v[1] = vec3(2,2,2);


	Ray ray(vec3(1,1,0),vec3(0,0,1));
	
	cout << temp.rayIntersect(ray) << endl;
	return 0;
}