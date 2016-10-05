#include <iostream>
#include "CTargaImage.h"


CTargaImage::CTargaImage()
{
	m_pImageData = NULL;

}

CTargaImage::~CTargaImage()
{
	Release();
}

void CTargaImage::SwapRedBlue()
{
	switch (m_colorDepth)
	{
	case 32:
	{
		UCHAR swap;
		rgba_t *pSource = (rgba_t*)m_pImageData;

		//loop for the number of pixels (objects)
		//and swap red and blue
		for (int pixel = 0; pixel < m_height * m_width; pixel++)
		{
			swap = pSource[pixel].b;
			pSource[pixel].b = pSource[pixel].r;
			pSource[pixel].r = swap;
		}
	}
		break;

	case 24:
	{
		UCHAR swap;
		rgb_t *pSource = (rgb_t*)m_pImageData;

		//loop for the number of pixels (objects)
		//and swap red and blue
		for (int pixel = 0; pixel < m_height * m_width; pixel++)
		{
			swap = pSource[pixel].b;
			pSource[pixel].b = pSource[pixel].r;
			pSource[pixel].r = swap;
		}
	}
		break;

	default:
		//ignore the other color depths
		break;
	}

}

bool CTargaImage::Load(const char *filename)
{
	//open the Targa (.tga) file
	FILE *pFile = fopen(filename, "rb");

	//if fopen is not successful then pFile will contain NULL
	if (!pFile)
		return false;

	//read the header all at once
	tgaheader_t tgaHeader;
	fread(&tgaHeader, //destination is address of the above object
		1,
		sizeof(tgaheader_t), //how many bytes?
		pFile);				 //from where

	//see if the imae type is one we support
	//TGATypes from CTargaImage.h
	if (((tgaHeader.imageTypeCode != TGA_RGB) &&
		(tgaHeader.imageTypeCode != TGA_GRAYSCALE) &&
		(tgaHeader.imageTypeCode != TGA_RGB_RLE) &&
		(tgaHeader.imageTypeCode != TGA_GRAYSCALE_RLE)) ||
		tgaHeader.colorMapType != 0
		)
	{
		//we dont support whatever file that we opened
		fclose(pFile); //so close it
		return false; // exit out of this function
	}

	//store the width and height of the image
	m_width = tgaHeader.width;
	m_height - tgaHeader.height;

	//make sure the image is not less than 24 bpp
	int colorMode = tgaHeader.bpp / 8;
	if (colorMode < 3)
	{
		fclose(pFile);
		return false;
	}

	//if we've made it to this point, we can now
	//start reading the pixel data
	m_imageSize = m_width * m_height * colorMode;
	m_pImageData = new unsigned char[m_imageSize];

	//move our read pointer past the offset and
	//read the image data
	if (tgaHeader.idlength > 0)
		fseek(pFile,	//which file
		SEEK_CUR, //move relative to the current read postion
		tgaHeader.idlength); //how far to move

	//read the uncompressed data
	if (tgaHeader.imageTypeCode == TGA_RGB ||
		tgaHeader.imageTypeCode == TGA_GRAYSCALE)
	{
		//uncompressed
		fread(m_pImageData, 1, m_imageSize, pFile);
	}
	else
	{
		//reading compressed data and uncompress it

		// read compressed image data (RLE compression)
		// ID holds whether or not the color that follows
		//   needs to be repeated, and if so, how many times
		//   or how many unique colors follow.
		//   if the 8th bit is a one, repeat, if zero unique
		unsigned char id;

		// length holds either how many to repeat the next color
		//  or how many unique colors follow
		unsigned char length;
		rgba_t color = { 0, 0, 0, 0 };
		unsigned int i = 0;

		while (i < m_imageSize) // iterate through m_imageData filling it in
		{
			id = fgetc(pFile); // read one byte from file

			if (id >= 128)
			{
				// we have a repeater, find how many times to repeat
				length = (unsigned char)(id - 127);

				// get the color to repeat
				// targa colors are in bgr or bgra format
				color.b = (unsigned char)fgetc(pFile);
				color.g = (unsigned char)fgetc(pFile);
				color.r = (unsigned char)fgetc(pFile);
				if (colorMode == 4) // 32 bit color
					color.a = (unsigned char)fgetc(pFile);


				// fill in the array repeat the color the number of
				//  times specified by length

				while (length > 0)
				{
					m_pImageData[i++] = color.b;
					m_pImageData[i++] = color.g;
					m_pImageData[i++] = color.r;
					if (colorMode == 4)
						m_pImageData[i++] = color.a;

					--length;
				}
			}

		}

		//close the fikle since we have already read
		//all of the header and pixel information
		fclose(pFile);

		//store the remainder of the image information
		//that's in the header, into the class member variables
		switch (tgaHeader.imageTypeCode)
		{
		case TGA_RGB:
		case TGA_RGB_RLE:
			if (colorMode == 3)
			{
				m_imageDataFormat = IMAGE_RGB;
				m_imageDataType = IMAGE_DATA_UNSIGNED_BYTE;
				m_colorDepth = 24;
			}
			else
			{
				m_imageDataFormat = IMAGE_RGBA;
				m_imageDataType = IMAGE_DATA_UNSIGNED_BYTE;
				m_colorDepth = 32;
			}

			break;
		case TGA_GRAYSCALE:
		case TGA_GRAYSCALE_RLE:
			m_imageDataFormat = IMAGE_LUMINANCE;
			m_imageDataType = IMAGE_DATA_UNSIGNED_BYTE;
			m_colorDepth = 8;
			break;
		}

		//some images are stored upside down, flip if necessary
		if ((tgaHeader.imageDesc & TOP_LEFT) == TOP_LEFT)
		{
			FlipVertical();
		}

		//swap the red and blue components of the image data
		SwapRedBlue();

		//at the end of this function, m_pImageData should
		//have all of te pixel data in the manner in which
		//we can use it.

		return true;
	}
}

void CTargaImage::Release()
{
	//deallocate the dynamic array
	delete[]  m_pImageData;
	m_pImageData = NULL;

}

bool CTargaImage::ConvertRGBtoRGBA(unsigned char alphaValue)
{
	// convert image from 24 bit to 32 bit color picture
	//   by adding an alpha byte to the pixel data

	if (m_colorDepth == 24 && m_imageDataFormat == IMAGE_RGB)
	{
		rgba_t *newImage = new rgba_t[m_width * m_height];
		if (!newImage)
			return false;

		rgba_t *dest = newImage;
		rgb_t *src = (rgb_t*)m_pImageData;

		for (int x = 0; x < m_height; x++)
		{
			for (int y = 0; y < m_width; y++)
			{
				dest->r = src->r;
				dest->g = src->g;
				dest->b = src->b;
				dest->a = alphaValue;

				src++; // increment 3 bytes (rgb_t)
				dest++; // increment 4 bytes (rgba_t)

			}
		}

		// delete original rgb image 
		delete[] m_pImageData;
		m_pImageData = (unsigned char*)newImage;

		m_colorDepth = 32;
		m_imageDataType = IMAGE_DATA_UNSIGNED_BYTE;
		m_imageDataFormat = IMAGE_RGBA;

		return true;
	}
	return false;
}


bool CTargaImage::ConvertRGBAtoRGB()
{
	// convert each pixel from 32 bit to 24 bit color
	//  essentially we just copy the rgb from the 32 bit
	//  image and leave behind the a (alpha).

	if (m_colorDepth == 32 && m_imageDataFormat == IMAGE_RGBA)
	{
		rgb_t *newImage = new rgb_t[m_width * m_height];
		if (!newImage)
			return false;

		rgb_t *dest = newImage;
		rgba_t *src = (rgba_t*)m_pImageData;


		//  loop to process each pixel
		for (int x = 0; x < m_height; x++)
		{
			for (int y = 0; y < m_width; y++)
			{
				dest->r = src->r;
				dest->g = src->g;
				dest->b = src->b;

				src++; // increment 4 bytes (rgba_t)
				dest++; // increment 3 bytes (rgb_t)

			}
		}

		// delete original rgba image 
		delete[] m_pImageData;
		// save the address of the new data
		//  to the global picture pointer
		m_pImageData = (unsigned char*)newImage;

		m_colorDepth = 24;
		m_imageDataType = IMAGE_DATA_UNSIGNED_BYTE;
		m_imageDataFormat = IMAGE_RGBA;

		return true;
	}
	return false;

}


bool CTargaImage::FlipVertical()
	{
		if (!m_pImageData) // m_pImageData == NULL
			return false;


		if (m_colorDepth == 32)
		{
			// allocate the temp swap memory
			rgba_t *tmpBits = new rgba_t[m_width];
			if (!tmpBits)
				return false;

			// calculating bytes per line
			int lineWidth = m_width * 4;

			rgba_t * top = (rgba_t*)m_pImageData;
			rgba_t * bottom = (rgba_t*)(m_pImageData + lineWidth * (m_height - 1));

			for (int i = 0; i < (m_height / 2); ++i)
			{
				memcpy(tmpBits, top, lineWidth);
				memcpy(top, bottom, lineWidth);
				memcpy(bottom, tmpBits, lineWidth);

				top = (rgba_t*)((unsigned char*)top + lineWidth);
				bottom = (rgba_t*)((unsigned char*)bottom - lineWidth);
			}

			delete[] tmpBits;
			tmpBits = NULL;
		}
		else if (m_colorDepth == 24)
		{
			// allocate the temp swap memory
			rgb_t *tmpBits = new rgb_t[m_width];
			if (!tmpBits)
				return false;

			// calculating bytes per line
			int lineWidth = m_width * 3;

			rgb_t * top = (rgb_t*)m_pImageData;
			rgb_t * bottom = (rgb_t*)(m_pImageData + lineWidth * (m_height - 1));

			for (int i = 0; i < (m_height / 2); ++i)
			{
				memcpy(tmpBits, top, lineWidth);
				memcpy(top, bottom, lineWidth);
				memcpy(bottom, tmpBits, lineWidth);

				top = (rgb_t*)((unsigned char*)top + lineWidth);
				bottom = (rgb_t*)((unsigned char*)bottom - lineWidth);
			}

			delete[] tmpBits;
			tmpBits = NULL;
		}

		return true;
	}