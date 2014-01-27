#ifndef _TGA_HANDLER_H_
#define _TGA_HANDLER_H_

#include "texture handler.h"

#include <string>
#include <vector>
#include <fstream>

#include <Windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <glut.h>

// this is based off a .tga texture loader found in: 
// this loader functions slightly differently to the loader present in: 

enum IMAGE_SETUPS
{
	BOTTOM_LEFT = 0x00,
	BOTTOM_RIGHT = 0x10,
	TOP_LEFT = 0x20,
	TOP_RIGHT = 0x30,
};

struct TGAHeader
{
	unsigned char IDLength;
	unsigned char colourMapType;
	unsigned char imageTypeCode;
	unsigned char colourMapSpec[5];
	unsigned short xAxisOrigin;
	unsigned short yAxisOrigin;
	unsigned short width;
	unsigned short height;
	unsigned char bitsPerPixel;
	unsigned char imageDesc;
};

enum TGAFileTypes
{
	TFT_NO_DATA = 0,
    TFT_INDEXED = 1,
    TFT_RGB = 2,
    TFT_GRAYSCALE = 3,
    TFT_RLE_INDEXED = 9,
    TFT_RLE_RGB = 10,
    TFT_RLE_GRAYSCALE = 11,
};


struct Px24
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};

struct Px32
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char alpha;
};

class TGAFile : public TextureHandler
{
public:
	TGAFile();
	~TGAFile();
	void printStats();
	bool loadTGA(std::string * fileName);
	
	void glMoveToGPU();
	void glActiveate();
	bool loadData(std::string * fileName);

private:
	TGAHeader info;
	// std::vector <unsigned char> data;
	// unsigned int width, height, bitsPerPixel, bytesPerPixel;
	// GLuint textureID;
	// bytes per pixel is used in the flipDataVertically() func
	void flipDataVertically();
	void loadUncompressed24Bit(std::ifstream * theFile);
	void loadUncompressed32Bit(std::ifstream * theFile);
	
	void loadRLE24Bit(std::ifstream * theFile);
	void loadRLE32Bit(std::ifstream * theFile);
};

#endif