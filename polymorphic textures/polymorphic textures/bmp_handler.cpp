#include "bmp_handler.h"

#include <iostream>
#include <string>
#include <fstream>

BmpFile::BmpFile()
{

}
	
BmpFile::~BmpFile()
{

	data.clear();
}

void BmpFile::printFileStats()
{
	std::cout << "the Magic number: " << magicNum.magic[0] << magicNum.magic[1] << std::endl;
	
	std::cout << "\nThe header\n";
	std::cout << "The file size: " << head.fileSize << " bytes\n";
	std::cout << "The creator 1 & 2 properties: " << head.creator1 << " & " << head.creator2 << std::endl;
	std::cout << "The offset: " << head.bmpOffset << std::endl;
	
	std::cout << "\nThe info header\n";
	std::cout << "The header size: " << info.headerSize << " bytes\n";
	std::cout << "Image width: " << info.width << " pixels\n";
	std::cout << "Image height: " << info.height << " pixels\n";
	std::cout << "Number of image planes: " << info.numberOfImagePlanes << std::endl;
	std::cout << "Bits per pixel: " << info.bitsPerPixel << std::endl;
	std::cout << "Compression type: " << info.compressionType << std::endl;
	std::cout << "Size of bytes: " << info.sizeInBytes << std::endl;
	std::cout << "Image width: " << info.imageWidth << std::endl;
	std::cout << "Image height: " << info.imageHeight << std::endl;
	std::cout << "Number of colours used: " << info.numColours << std::endl;
	std::cout << "Number of imported colours: " << info.numColours << std::endl;
}

bool BmpFile::loadData(std::string * fileName)
{
	std::ifstream theFile(fileName->c_str(), std::ios::binary);
	this->fileName = * fileName;
	if (!theFile.good())
	{
		std::cout << "Couldn't file the file\n";
		return false;
	}
	
	theFile.read(reinterpret_cast<char *>(&magicNum), sizeof(magicNumber));
	theFile.read(reinterpret_cast<char *>(&head), sizeof(fileHeader));
	theFile.read(reinterpret_cast<char *>(&info), sizeof(infoHeader));
	// now to pass the file stream to an appropiate loading function
	printFileStats();
	width = info.width;
	height = info.height;
	bitsPerPixel = info.bitsPerPixel;
	bytesPerPixel = bitsPerPixel / 8;
	// rember it's bgr not rgb!
	std::cout << "After reading in the headers the reader is " << theFile.tellg() << " bytes in.\n The offset for the start of the imaeg data is: " << head.bmpOffset << " bytes in.\n";  
	theFile.seekg(head.bmpOffset); // move to reader to the start of image data
	std::cout << "After moving the reader, its at " << theFile.tellg() << " bytes in.\n";  

	std::cout << "This is a " << info.bitsPerPixel << " bit bitmap file\n";
	

	if (info.bitsPerPixel == 8) 
	{
		std::cout << "This is a 8 bit bitmap image, ony 24 bit and 32 bit will be supported\n";
		return false;
	}
	else if(info.bitsPerPixel == 16) 
	{
		std::cout << "This is a 16 bit bitmap image, ony 24 bit and 32 bit will be supported\n";
		return false;
	}
	else if (info.bitsPerPixel == 24) // 24 bit colour
	{
		if (info.compressionType == BMP_RGB) // uncompressed
		{
			return loadUncompressed24Bit(&theFile);
		}
		else if (info.compressionType == BMP_RLE4) // 4 bit rle commands
		{
			std::cout << "The file uses 4 bit RLE compression which is unsupported.\n";
			return false;
		}
		else if (info.compressionType == BMP_RLE8) // 8 bit rle commands
		{
			std::cout << "The file uses 4 bit RLE compression.\n";
		}
		else if (info.compressionType == BMP_BITFIELDS)
		{
			std::cout << "The file uses bitfields compression which is unsupported.\n";
			return false;
		}
		else if (info.compressionType == BMP_JPEG)
		{
			std::cout << "The file uses jpeg compression which is unsupported.\n";
			return false;
		}
		else if (info.compressionType == BMP_PNG)
		{
			std::cout << "The file uses png compression which is unsupported.\n";
			return false;
		}
		else if (info.compressionType == BMP_ALPHABITFIELDS)
		{
			std::cout << "The file uses alpha bit fields compression which is unsupported.\n";
			return false;
		}
		else
		{
			std::cout << "The file uses unknown compression which is unsupported.\n";
			return false;
		}
		
	}
	else if(info.bitsPerPixel == 32) // 32 bit colour
	{
		if (info.compressionType == BMP_RGB) // uncompressed
		{
			return loadUncompressed32Bit(&theFile);
		}
		else if (info.compressionType == BMP_RLE4) // 4 bit rle commands
		{
			std::cout << "The file uses 4 bit RLE compression which is unsupported.\n";
			return false;
		}
		else if (info.compressionType == BMP_RLE8) // 8 bit rle commands
		{
			std::cout << "The file uses 4 bit RLE compression.\n";
		}
		else if (info.compressionType == BMP_BITFIELDS)
		{
			std::cout << "The file uses bitfields compression which is unsupported.\n";
			return false;
		}
		else if (info.compressionType == BMP_JPEG)
		{
			std::cout << "The file uses jpeg compression which is unsupported.\n";
			return false;
		}
		else if (info.compressionType == BMP_PNG)
		{
			std::cout << "The file uses png compression which is unsupported.\n";
			return false;
		}
		else if (info.compressionType == BMP_ALPHABITFIELDS)
		{
			std::cout << "The file uses alpha bit fields compression which is unsupported.\n";
			return false;
		}
		else
		{
			std::cout << "The file uses unknown compression which is unsupported.\n";
			return false;
		}
	}

	theFile.close();
	return true;
}

bool BmpFile::loadUncompressed24Bit(std::istream * theFile)
{
	std::cout << "Now loading the data.\nnote that this file is a uncompressed 24 bit .bmp file (data stored as BGR)\n";
	Pixel24Bit tmpPX;
	__int32 padding; // 4 bytes are added to the end of each row (according to: http://en.wikipedia.org/wiki/BMP_file_format#Pixel_storage)
	for (int i = 0; i < info.height; i++)
	{
		std::cout << "Loading row " << i + 1 << " of " << info.height << std::endl;
		for (int j = 0; j < info.width; j++)
		{
			// std::cout << "loading pixel " << i << " of " << info.width * info.height << std::endl;
			theFile->read(reinterpret_cast<char *>(&tmpPX), sizeof Pixel24Bit);
			// std::cout << "stats for the current pixel \n(note that the data is stored in the file as BGR NOT RGB):\n";
			// std::cout << "The value for the blue component of this pixel: " << static_cast<short>(tmpPX.blue) << std::endl;
			// std::cout << "The value for the green component of this pixel: " << static_cast<short>(tmpPX.green) << std::endl;
			// std::cout << "The value for the red component of this pixel: " << static_cast<short>(tmpPX.red) << std::endl;
			data.push_back(tmpPX.red);
			data.push_back(tmpPX.green);
			data.push_back(tmpPX.blue);
			// std::cout << "the data is now being put into the std::vector in RGB format\n";
		}
		// theFile->read(reinterpret_cast<char *>(&padding), sizeof(__int32));
	}
	return true;
}

bool BmpFile::loadUncompressed32Bit(std::istream * theFile)
{
	std::cout << "Now loading the data.\nnote that this file is a uncompressed 32 bit .bmp file (data stored as BGRA)\n";
	Pixel32Bit tmpPX;
	__int32 padding; // 4 bytes are added to the end of each row (according to: http://en.wikipedia.org/wiki/BMP_file_format#Pixel_storage)
	for (int i = 0; i < info.height; i++)
	{
		std::cout << "Loading row " << i + 1 << " of " << info.height << std::endl;
		for (int j = 0; j < info.width; j++)
		{
			// std::cout << "loading pixel " << i << " of " << info.width * info.height << std::endl;
			theFile->read(reinterpret_cast<char *>(&tmpPX), sizeof Pixel32Bit);
			// std::cout << "stats for the current pixel \n(note that the data is stored in the file as BGR NOT RGB):\n";
			// std::cout << "The value for the blue component of this pixel: " << static_cast<short>(tmpPX.blue) << std::endl;
			// std::cout << "The value for the green component of this pixel: " << static_cast<short>(tmpPX.green) << std::endl;
			// std::cout << "The value for the red component of this pixel: " << static_cast<short>(tmpPX.red) << std::endl;
			data.push_back(tmpPX.red);
			data.push_back(tmpPX.green);
			data.push_back(tmpPX.blue);
			data.push_back(tmpPX.alpha);
			// std::cout << "the data is now being put into the std::vector in RGB format\n";
		}
		theFile->read(reinterpret_cast<char *>(&padding), sizeof(__int32));
		// no padding on 32 images?
	}
	return false;
}

void BmpFile::glMoveToGPU()
{
	// determine the image set up

	if (info.bitsPerPixel == 32)
	{
		unsigned char * transfer;
		transfer = new unsigned char[data.size()];
		for (int i = 0; i < data.size(); i++)
		{
			(* (transfer + i)) = data[i];
		}


		glGenTextures(1, &id); // make room for a texture
		glBindTexture(GL_TEXTURE_2D, id); // select the texture this class is responsable for the one OpenGL is manipulating / using
	
		glTexImage2D(GL_TEXTURE_2D, // 1d for cel shading other wise use 2d
				 0, // no mipmaps for now
				 GL_RGBA, // internal format (how it's stored on the gpu)
				 info.width,
				 info.height,
				 0, // border
				 GL_RGBA, // how it's stored inside the array being passed
				 GL_UNSIGNED_BYTE, // char variable is used to store the data, which takes up one byte per char
				 transfer);
		delete[] transfer;
	}
	else if (info.bitsPerPixel == 24)
	{
		unsigned char * transfer;
		transfer = new unsigned char[data.size()];

		for (int i = 0; i < data.size(); i++)
		{
			(* (transfer + i)) = data[i];
		}

		glGenTextures(1, &id); // make room for a texture
		glBindTexture(GL_TEXTURE_2D, id); // select the texture this class is responsable for the one OpenGL is manipulating / using
	
		glTexImage2D(GL_TEXTURE_2D, // 1d for cel shading other wise use 2d
				 0, // no mipmaps for now
				 GL_RGB, // internal format (how it's stored on the gpu)
				 info.width,
				 info.height,
				 0, // border
				 GL_RGB, // how it's stored inside the array being passed
				 GL_UNSIGNED_BYTE, // char variable is used to store the data, which takes up one byte per char
				 transfer);
		delete[] transfer;
	}
}

void BmpFile::glActiveate()
{
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