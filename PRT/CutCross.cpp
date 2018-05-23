#include <string>
#include <iostream>

#include "CImg/CImg.h"
#include "glm/glm.hpp"

#include "toolFunction.h"

using glm::vec2;
using glm::ivec2;
using std::string;

using namespace cimg_library;
//0 ~6
// +y , -x ,-z, +x, -y,+z

string filenames[] = {"/top.bmp", "/left.bmp", "/front.bmp", "/right.bmp", "/bottom.bmp", "/back.bmp"};

struct Square
{
	ivec2 lu;//left up
	ivec2 rd;// right down
};

int main2(int argc, char** argv)
{

	// argv[1] = "LightingCube/grace";
	// argv[2] = "grace.bmp";

	int width,height;
	string temp(argv[1]);
	string temp2(argv[2]);

	string filepath = temp +"/"+ temp2;


	//GLubyte *pixels = loadBMP(filepath,width,height);
	CImg<unsigned char> src(filepath.c_str());

	width = src.width();
	height = src.height();
	

/*	if(!pixels)
	{
		std::cout << "Image open failed." << std::endl;
	}*/
	if(src.empty())
	{
		std::cout << "Image open failed." << std::endl;

	}

	Square sq[6];

	sq[0].lu = vec2(width/3.0,0);
	sq[0].rd = vec2(2 * width/3,height/4);

	sq[1].lu = vec2(0,height/4);
	sq[1].rd = vec2(width/3,height/2);

	sq[2].lu = vec2(width/3,height/4);
	sq[2].rd = vec2(2 * width/3,height/2);

	sq[3].lu = vec2(2 * width/3,height/4);
	sq[3].rd = vec2(width,height/2);

	sq[4].lu = vec2(width/3,height/2);
	sq[4].rd = vec2(2 * width/3,3 * height/4);

	sq[5].lu = vec2(width/3,3 * height/4);
	sq[5].rd = vec2(2 * width/3,height);

	for(int k = 0;k < 6;++k)
	{
		CImg<unsigned char> out(width/3,height/4,3,3);

		for(int y = sq[k].lu[1];y < sq[k].rd[1]; ++y)
		{
			for(int x = sq[k].lu[0]; x < sq[k].rd[0]; ++x)
			{
			//	bits[FI_RGBA_RED] = FreeImage_GetPixelColor(dib, x, y, FI_RGBA_RED);

				float xoff = (float)((x - sq[k].lu[0]));
				
				float yoff = (float)((y - sq[k].lu[1]));
				if(k != 5)
				{
					yoff = height/4 - yoff  - 1;
				}
				if( k == 5)
				{
					xoff = width/3 - xoff - 1;
				}
				//std::cout << xoff << ' ' << yoff <<std::endl;
				//system("pause");

				int index = 3 * width  * height - 3*(y * width + x);

			/*	out.set_linear_atXY(pixels[index + 2],xoff,yoff,0,0);
				out.set_linear_atXY(pixels[index + 1],xoff,yoff,0,1);
				out.set_linear_atXY(pixels[index],xoff,yoff,0,2);*/

				out.set_linear_atXY(src.atXY(x,y,0,0),xoff,yoff,0,0);
				out.set_linear_atXY(src.atXY(x,y,0,1),xoff,yoff,0,1);
				out.set_linear_atXY(src.atXY(x,y,0,2),xoff,yoff,0,2);





			/*	out.atXY(xoff,yoff,0,0) = pixels[index];
				out.atXY(xoff,yoff,0,1) = pixels[index + 1];
				out.atXY(xoff,yoff,0,2) = pixels[index + 2];*/

				

			}

		}

		string outpath = temp + filenames[k];
		out.save(outpath.c_str());
	}

	return 0;
}