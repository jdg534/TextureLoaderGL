#include <string>
#include <vector>

#include <Windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <glut.h>

#include "texture handler.h"

#ifndef _BMP_READER_H_
#define _BMP_READER_H_


struct magicNumber
{
	char magic[2];
};

struct fileHeader
{
	unsigned __int32 fileSize;
	unsigned __int16 creator1;
	unsigned __int16 creator2;
	unsigned __int32 bmpOffset; // offset to the actual image data
};

struct infoHeader
{
	unsigned __int32 headerSize;
	signed __int32 width;
	signed __int32 height;
	unsigned __int16 numberOfImagePlanes;
	unsigned __int16 bitsPerPixel;
	unsigned __int32 compressionType;
	unsigned __int32 sizeInBytes;
	signed __int32 imageWidth;
	signed __int32 imageHeight;
	unsigned __int32 numColours;
	unsigned __int32 numImportedColour;
};

struct Pixel24Bit
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};

struct Pixel32Bit
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char alpha;
};


// http://en.wikipedia.org/wiki/BMP_file_format#Pixel_storage
enum compresion_types
{
	BMP_RGB = 0,
	BMP_RLE8,
	BMP_RLE4,
	BMP_BITFIELDS,
	BMP_JPEG,
	BMP_PNG,
	BMP_ALPHABITFIELDS,
};

class BmpFile : public TextureHandler
{
public:
	BmpFile();
	~BmpFile();
	void printFileStats();
	bool loadData(std::string * fileName);
	void glMoveToGPU();
	void glActiveate();
private:
	magicNumber magicNum;
	fileHeader head;
	infoHeader info;
	
	// GLuint texId; 

	// std::vector<unsigned char> data;

	bool loadUncompressed24Bit(std::istream * theFile);
	bool loadUncompressed32Bit(std::istream * theFile);
	
};

#endif