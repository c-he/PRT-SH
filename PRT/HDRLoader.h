#ifndef HDRLOADER_H_
#define HDRLOADER_H_
class HDRLoaderResult 
{
	public:
	int width, height;
	// each pixel takes 3 float32, each component can be of any value...
	float *cols;
};

class HDRLoader 
{
	public:
		bool load(const char *fileName, HDRLoaderResult &res);
};


#endif