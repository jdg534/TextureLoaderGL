#include "tga_heandler.h"
#include <string>
#include <iostream>
#include <fstream>

#include <Windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <glut.h>

TGAFile::TGAFile()
{

}

TGAFile::~TGAFile()
{
	glDeleteTextures(1, &id);
}

bool TGAFile::loadData(std::string * fileName)
{
	return loadTGA(fileName);
}

void TGAFile::printStats()
{
	std::cout << "Stats for the file .tga:\n\n";
	std::cout << "The ID length: " << static_cast<short>(info.IDLength) << std::endl;
	std::cout << "The colour map type: " << static_cast<short> (info.colourMapType) << std::endl;
	std::cout << "Image type code: " << static_cast<short>(info.imageTypeCode) << std::endl;
	std::cout << "Colour map spec: ";
	for (int i = 0; i < 5; i++)
	{
		std::cout << static_cast<short>(info.colourMapSpec[i]) << " ";
	}
	std::cout << std::endl;
	std::cout << "X axis origin: " << info.xAxisOrigin << std::endl;
	std::cout << "Y axis origin: " << info.yAxisOrigin << std::endl;
	std::cout << "Image width: " << info.width << std::endl;
	std::cout << "Image height: " << info.height << std::endl;
	std::cout << "bits per pixel: " << static_cast<short>(info.bitsPerPixel) << std::endl;
	std::cout << "Image Description: " << static_cast<short>( info.imageDesc) << std::endl;
}

bool TGAFile::loadTGA(std::string * fileName)
{
	std::ifstream theFile(fileName->c_str(), std::ios::binary);
	if (!theFile.good())
	{
		return false;
	}
	this->fileName = *fileName;

	theFile.read(reinterpret_cast<char *>(&info),sizeof(TGAHeader));

	bitsPerPixel = static_cast<int>(info.bitsPerPixel);
	bytesPerPixel = bitsPerPixel / 8;
	width = info.width;
	height = info.height;

	
	printStats();

	if (bytesPerPixel < 3)
	{
		// 24 bit and 32 textures only!
		return false;
	}

	unsigned int sizeInBytes = width * height * bytesPerPixel;

	data.resize(sizeInBytes);

	if (info.IDLength > 0)
	{
		// skip ahead to the data
		theFile.ignore(info.IDLength);
	}

	if (info.imageTypeCode == TFT_RGB)
	{
		if (bitsPerPixel == 24)
		{
			loadUncompressed24Bit(&theFile);
			
		}
		else if (bitsPerPixel == 32)
		{
			loadUncompressed32Bit(&theFile);
			
		}
	}
	else if (info.imageTypeCode == TFT_RLE_RGB)
	{
		unsigned int sizeInBytes = width * height * bytesPerPixel;
		data.resize(sizeInBytes);

		if (bitsPerPixel == 24)
		{
			loadRLE24Bit(&theFile);
			
		}
		else if (bitsPerPixel == 32)
		{
			loadRLE32Bit(&theFile);
			
		}
	}

	// use the data in side the header to determine if the image data needs to be fliped
	if ((info.imageDesc & TOP_LEFT) == TOP_LEFT)
	{
		flipDataVertically();
	}

	theFile.close();
	return true;
}

void TGAFile::flipDataVertically()
{
	std::vector<unsigned char> flippedData;
	flippedData.reserve(data.size());

	int byteCount = bytesPerPixel;

	for (int row = height; row >= 0; row--)
	{
		unsigned char * rowData = &data[row * width * byteCount];

		// now to store the data in it's correct format
		for (unsigned int i = 0; i < width * byteCount; ++i)
		{
			flippedData.push_back(*rowData);
			rowData++;
		}
	}

	// finally to replace the original image with the flipped version
	data.assign(flippedData.begin(), flippedData.end());
}

void TGAFile::loadUncompressed24Bit(std::ifstream * theFile)
{
	/* (the old code)
	unsigned int size = data.size();

	theFile->read(reinterpret_cast<char *>(&data[0]), size); // all of the file I/O for reading done in one function call
	// but the data is in bgr format, rgb is what's prefered

	// so swap the values
	for (unsigned int i = 0; i < size; i += bytesPerPixel)
	{
		char tmpBlue = data[i]; // keep a copy of the blue component
		data[i] = data[i + 2]; // make the blue value store the red value
		data[i + 2] = tmpBlue; // make the red value store the blue value
	}

	*/

	// new code
	
	Px24 tmpPX;
	
	data.clear();
	data.resize(0);

	for (unsigned int i = 0; i < height; i++)
	{
		std::cout << "Reading line of 24 bit non compressed pixel data (.tga)\n row " << i + 1 << " of " << height << std::endl;
		
		for (unsigned int j = 0; j < width; j++)
		{
			theFile->read(reinterpret_cast<char *>(&tmpPX), sizeof Px24);
			data.push_back(tmpPX.red);
			data.push_back(tmpPX.green);
			data.push_back(tmpPX.blue);
			// (old debug code) // std::cout << "R: " << static_cast<short>(tmpPX.red) << std::endl;
			// (old debug code) // std::cout << "G: " << static_cast<short>(tmpPX.green) << std::endl;
			// (old debug code) // std::cout << "B: " << static_cast<short>(tmpPX.blue) << std::endl;
		}
	}
}

void TGAFile::loadUncompressed32Bit(std::ifstream * theFile)
{
	// basically the same as loadUncompressed24Bit()
	// but it's BGRA so can actually reuse the same code, since only B & R get swaped
	// and bytesPerPixel would store 4 instead of 3 since the file would be using 32 colour in this case
	unsigned int size = data.size();

	theFile->read(reinterpret_cast<char *>(&data[0]), size); // all of the file I/O for reading done in one function call
	// but the data is in bgr format, rgb is what's prefered

	// so swap the values
	for (unsigned int i = 0; i < size; i += bytesPerPixel)
	{
		char tmpBlue = data[i]; // keep a copy of the blue component
		data[i] = data[i + 2]; // make the blue value store the red value
		data[i + 2] = tmpBlue; // make the red value store the blue value
	}
}
	
void TGAFile::loadRLE24Bit(std::ifstream * theFile)
{
	unsigned int numPx = width * height;
	unsigned int currentPx = 0;
	unsigned int currentByte = 0;

	std::vector<unsigned char> pixelBuffer;

	do
	{
		unsigned char segmentHeader = 0;
		theFile->read(reinterpret_cast<char *>(&segmentHeader),sizeof( unsigned char));

		if (segmentHeader < 128)
		{
			segmentHeader++;

			for (short i = 0; i < segmentHeader; i++)
			{
				theFile->read(reinterpret_cast<char *>(&pixelBuffer[0]), bytesPerPixel);

				data[currentByte] = pixelBuffer[2]; // red part of pixel
				data[currentByte + 1] = pixelBuffer[1]; // green
				data[currentByte + 2] = pixelBuffer[0]; //  blue


				/* // this bit should be included in the 32 bit colour version of the function

				if (bytesPerPixel == 4)
				{
					data[currentByte + 3] = pixelBuffer[3];
				}

				*/
				currentByte += bytesPerPixel;
				currentPx++;

				if (currentPx > numPx) // (read too many pixels)
				{
					std::cerr << "The RLE .tga file has been misread!\n";
					return;
				}
			}
		}
		else
		{
			segmentHeader -= 127;

			theFile->read(reinterpret_cast<char *>(&pixelBuffer[0]), bytesPerPixel);
			for (short counter = 0; counter < segmentHeader; counter++)
			{
				data[currentByte] = pixelBuffer[2];
				data[currentByte + 1] = pixelBuffer[1];
				data[currentByte + 2] = pixelBuffer[0];


				/* // a bit to add in the 32 bit colour version
				if (bytesPerPixel == 4)
				{
					data[currentByte + 3] = pixelBuffer[3];
				}
				*/

				currentByte += bytesPerPixel;
				currentPx++;

				if (currentPx > numPx) // read more pixels than was ment to be in the image
				{
					std::cerr << "The RLE .tga file has been misread\n";
					return;
				}
			}
		}

	} while (currentPx < numPx);
}

void TGAFile::loadRLE32Bit(std::ifstream * theFile)
{
	unsigned int numPx = width * height;
	unsigned int currentPx = 0;
	unsigned int currentByte = 0;

	std::vector<unsigned char> pixelBuffer;

	do
	{
		unsigned char segmentHeader = 0;
		theFile->read(reinterpret_cast<char *>(&segmentHeader),sizeof( unsigned char));

		if (segmentHeader < 128)
		{
			segmentHeader++;

			for (short i = 0; i < segmentHeader; i++)
			{
				theFile->read(reinterpret_cast<char *>(&pixelBuffer[0]), bytesPerPixel);

				data[currentByte] = pixelBuffer[2]; // red part of pixel
				data[currentByte + 1] = pixelBuffer[1]; // green
				data[currentByte + 2] = pixelBuffer[0]; //  blue


				// this bit should be included in the 32 bit colour version of the function

				if (bytesPerPixel == 4)
				{
					data[currentByte + 3] = pixelBuffer[3];
				}
				
				currentByte += bytesPerPixel;
				currentPx++;

				if (currentPx > numPx) // (read too many pixels)
				{
					std::cerr << "The RLE .tga file has been misread!\n";
					return;
				}
			}
		}
		else
		{
			segmentHeader -= 127;

			theFile->read(reinterpret_cast<char *>(&pixelBuffer[0]), bytesPerPixel);
			for (short counter = 0; counter < segmentHeader; counter++)
			{
				data[currentByte] = pixelBuffer[2];
				data[currentByte + 1] = pixelBuffer[1];
				data[currentByte + 2] = pixelBuffer[0];


				// a bit to add in the 32 bit colour version
				if (bytesPerPixel == 4)
				{
					data[currentByte + 3] = pixelBuffer[3];
				}
				

				currentByte += bytesPerPixel;
				currentPx++;

				if (currentPx > numPx) // read more pixels than was ment to be in the image
				{
					std::cerr << "The RLE .tga file has been misread\n";
					return;
				}
			}
		}

	} while (currentPx < numPx);
}

void TGAFile::glMoveToGPU()
{
	std::cout << "Moving the texture data onto the GPU\n";
	if (bitsPerPixel == 24)
	{
		unsigned char * transfer;
		const unsigned int size = data.size();
		std::cout << "the amount of bytes for the texture: " << size << std::endl;

		transfer = new unsigned char[size];
		for (int i = 0; i < data.size(); i++)
		{
			(* (transfer + i)) = data[i];
		}
		std::cout << "It's a 24 bit texture\n";
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexImage2D(GL_TEXTURE_2D,
					0,
					GL_RGB,
					info.width,
					info.height,
					0, // clamped boarder?
					GL_RGB,
					GL_UNSIGNED_BYTE,
					transfer);
		
		//Bottom
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		std::cout << "The texture has been moved to the GPU and has a texture ID of: " << id << std::endl;

		delete[] transfer;
		data.clear(); // the above call to glTexImage2D copies the data to the gpu
		// so so remove it from ram
	}
	else if (bitsPerPixel == 32)
	{
		unsigned char * transfer;
		transfer = new unsigned char[data.size()];
		for (int i = 0; i < data.size(); i++)
		{
			(* (transfer + i)) = data[i];
		}
		std::cout << "It's a 32 bit texture\n";
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D,
					0,
					GL_RGBA,
					info.width,
					info.height,
					0, // clamped boarder?
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					transfer);
		std::cout << "The texture has been moved to the GPU and has a texture ID of: " << id << std::endl;

		delete[] transfer;
		data.clear(); // the above call to glTexImage2D copies the data to the gpu
		// so so remove it from ram
	}
}

void TGAFile::glActiveate()
{
	std::cout << "Texture: " << id << "\n has been actervated\n";
	if (!glIsEnabled(GL_TEXTURE_2D))
	{
		glEnable(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // setup filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // more filtering
	// nearest nabor of liner interpolation?
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}