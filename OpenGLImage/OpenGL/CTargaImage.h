#ifndef _TARGA_LIB
#define _TARGA_LIB

enum TGATypes
{
	TGA_NODATA = 0,
	TGA_INDEXED = 1,
	TGA_RGB = 2,
	TGA_GRAYSCALE = 3,
	TGA_INDEXED_RLE = 9,
	TGA_RGB_RLE = 10,
	TGA_GRAYSCALE_RLE = 11

};

//IMAGE DATA FORMATS
#define IMAGE_RGB		0
#define IMAGE_RGBA		1
#define IMAGE_LUMINANCE 2

//image data types
#define IMAGE_DATA_UNSIGNED_BYTE	0

//Pixel data transfer from file to screen:
//these masks are AND'd with the image Desc in the
//TGA header.
//bit 4 is LEFT-TO-RIGHT ordering
//bit 5 is TOP-TO-BOTTOM
#define BOTTOM_LEFT  0x00 //first pixel is bottom left corner
#define BOTTOM_RIGHT 0x10 //first pixel is bottom right corner
#define TOP_LEFT	 0x20 //first pixel is top left corner
#define TOP_RIGHT	 0x30 //first pixel is top right corner

typedef unsigned short USHORT;
typedef unsigned char UCHAR;
typedef unsigned char ULONG;

//TGA header (information about the image data)
struct tgaheader_t
{
	UCHAR idlength; //offset after the header before the data
	UCHAR colorMapType;
	UCHAR imageTypeCode; //TGATypes - compressed, uncompressed, RGA, etc...
	UCHAR colorMapSpec[5];
	USHORT xOrgin;
	USHORT yOrgin;
	USHORT width;
	USHORT height;
	UCHAR bpp; //bits per pixel (8, 16, 24, 32) ?
	UCHAR imageDesc; //image upside down? bits 4 and 5

};

//structures for pixel data (RGB, RGBA)
struct rgba_t //32 bit color
{
	UCHAR r;
	UCHAR g;
	UCHAR b;
	UCHAR a;
};

struct rgb_t //24 bit color
{
	UCHAR r;
	UCHAR g;
	UCHAR b;
};

class CTargaImage
{
private:
	UCHAR m_colorDepth; //8, 16, 24, 32 bpp
	UCHAR m_imageDataType;
	UCHAR m_imageDataFormat;
	UCHAR m_DataFormat;
	UCHAR *m_pImageData;
	USHORT m_width;
	USHORT m_height;
	ULONG m_imageSize;

	//most libraries use RGB as the image data order
	//targa stres the image in BGR order
	//so we must chang it from BGR --> RGB
void SwapRedBlue();

public:
	CTargaImage();
	~CTargaImage();

	//loading and unloading of the image
	bool Load(const char *filename);
	void Release();

	//flip the image vertically (if the image is upside down)
	bool FlipVertical();

	//accessors
	USHORT getWidth() { return m_width; }
	USHORT getHeight() { return m_height; }
	UCHAR getImageFormat() { return m_imageDataFormat; }

	//convert from RGBA and vice versa
	bool ConvertRGBAtoRGB();
	bool ConvertRGBtoRGBA(unsigned char alphaValue);

	//return the current image data
	UCHAR *GetImage() { return m_pImageData; }
};


#endif