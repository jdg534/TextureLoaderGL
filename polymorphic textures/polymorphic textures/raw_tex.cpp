#include "raw_tex.h"

#include <vector>
#include <fstream>
#include <string>
#include <iostream>

#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include "glut.h"

RawTexure::RawTexure()
{

}

RawTexure::~RawTexure()
{
	dataLoadForm.clear();
	delete [] dataUseForm; // once on the gpu you don't need to store the data
}

bool RawTexure::loadData(std::string * fn)
{
	std::ifstream theFile(fn->c_str(), std::ios::binary);

	if (!theFile.good())
	{
		return false; // fasle = fail!
	}
	
	int sizeInBytes;
	theFile.seekg(std::ios::end);
	
	sizeInBytes = theFile.tellg();
	std::cout << "The raw file is: " << sizeInBytes << "bytes in size\n";
	int numPixels = sizeInBytes / 3; // 24 bit image data (3 bytes per pixel)
	std::cout << "The file has " << numPixels << " pixels\n";
	width = numPixels / 2;
	height = numPixels / 2;
	std::cout << "Assuming that the image is an exact square:\n" << "width: " << width << " pixels\nheight: " << height << " pixels\n";

	theFile.seekg(std::ios::beg);
	
	return true; // true = pass!
}

void RawTexure::glActiveate()
{
	if (!glIsEnabled(GL_TEXTURE_2D))
	{
		glEnable(GL_TEXTURE_2D); //  enable textureing
	}
	
	glBindTexture(GL_TEXTURE_2D, glTexId); // select the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // setup filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // more filtering
	// nearest nabor of liner interpolation?
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void RawTexure::glMoveToGPU()
{
	glGenTextures(1, &glTexId); // make room for a texture
	glBindTexture(GL_TEXTURE_2D, glTexId); // select the texture this class is responsable for the one OpenGL is manipulating / using
	glTexImage2D(GL_TEXTURE_2D, // 1d for cel shading other wise use 2d
				 0, // no mipmaps for now
				 GL_RGB, // internal format (how it's stored on the gpu)
				 width,
				 height,
				 0, // border
				 GL_RGB, // how it's stored inside the array being passed
				 GL_UNSIGNED_BYTE, // char variable is used to store the data, which takes up one byte per char
				 dataUseForm);
	
}