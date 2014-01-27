#ifndef __RAW__TEX__
#define __RAW__TEX__

#include <vector>
#include <fstream>
#include <string>
#include <gl\gl.h>

struct pixel24Bit
{
	unsigned char r, g, b;
};

class RawTexure
{
public:
	RawTexure();
	~RawTexure();
	bool loadData(std::string * fn);
	void glMoveToGPU();
	void glActiveate();
private:
	std::vector<char> dataLoadForm;
	char * dataUseForm;

	unsigned int width, height;
	GLuint glTexId; 
};


#endif