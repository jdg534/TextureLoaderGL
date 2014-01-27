#ifndef _TEXTURE_HANDLER_H_
#define _TEXTURE_HANDLER_H_

#include <string>
#include <vector>

#include <gl\gl.h>
#include <gl\glu.h>

class TextureHandler
{
public:
	TextureHandler();
	~TextureHandler();
	virtual void glMoveToGPU();
	virtual void glActiveate();
	virtual bool loadData(std::string * fileName);
protected:
	std::string fileName;
	GLuint id;
	unsigned int width, height,  bitsPerPixel, bytesPerPixel;
	std::vector<unsigned char> data;
};

#endif