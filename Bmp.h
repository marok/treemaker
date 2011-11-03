#ifndef _BMP_H
#define _BMP_H
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
using namespace std;

class Bmp
{
private:
	bool dataAlocated;
public:
	bool isRGBA;
	unsigned char *data;
	Bmp ()
	{
		dataAlocated = false;
	}
	~Bmp ()
	{
		if (dataAlocated) {
			free (data);
		}
	}
	struct BMPHeader
	{
		char bfType[2];		/* "BM" */
		int bfSize;		/* Size of file in bytes */
		int bfReserved;		/* set to 0 */
		int bfOffBits;		/* Byte offset to actual bitmap data (= 54) */
		int biSize;		/* Size of BITMAPINFOHEADER, in bytes (= 40) */
		int biWidth;		/* Width of image, in pixels */
		int biHeight;		/* Height of images, in pixels */
		short biPlanes;		/* Number of planes in target device (set to 1) */
		short biBitCount;	/* Bits per pixel (24 in this case) */
		int biCompression;	/* Type of compression (0 if no compression) */
		int biSizeImage;	/* Image size, in bytes (0 if no compression) */
		int biXPelsPerMeter;	/* Resolution in pixels/meter of display device */
		int biYPelsPerMeter;	/* Resolution in pixels/meter of display device */
		int biClrUsed;		/* Number of colors in the color table (if 0, use
				   maximum allowed by biBitCount) */
		int biClrImportant;	/* Number of important colors.  If 0, all colors
				   are important */
	};

	BMPHeader header;

	/**
	*	Prints BMP header to stdout
	**/
	void print ()
	{
		printf ("%c%c\n", header.bfType[0], header.bfType[1]);
		printf ("bfSize %d\n", header.bfSize);
		printf ("bfReserved %d\n", header.bfReserved);
		printf ("bfOffBits %d\n", header.bfOffBits);
		printf ("biSize %d\n", header.biSize);
		printf ("biWidth %d\n", header.biWidth);
		printf ("biHeight %d\n", header.biHeight);
	}
	/**
	*	Makes RGBA from RGB bitmap (white color as alpha)
	**/
	void addAlpha ()
	{
		if (dataAlocated == false)
			return;
		int height = header.biHeight;

		int width = header.biWidth;

		int newSize = 4 * height * width;

		unsigned char *newData = (unsigned char *) malloc (newSize);

		unsigned char *p = data;

		unsigned char *q = newData;

		for (int i = 0; i < height * width; i++) {
			//is white
			unsigned char alpha = 1;

			if (*p == 0 && *(p + 1) == 0 && *(p + 2) == 0) {
				alpha = 0;
			}
			q[0] = p[0];
			q[1] = p[1];
			q[2] = p[2];
			q[3] = alpha;
			p += 3;
			q += 4;
		}
		free (data);
		data = newData;
	}

	bool load (char *filename)
	{
#define ERROR_CHECK(condition,message) \
 if(condition){\
  fprintf(stderr,"Bmp::load(), ");\
  fprintf(stderr, message);\
  fprintf(stderr,"\n");\
  return false;\
 }
		ERROR_CHECK (filename == NULL, "filename is null");

		FILE *file = fopen (filename, "r");

		ERROR_CHECK (file == NULL, "cannot open file");

		fread (header.bfType, 1, 2, file);
		ERROR_CHECK(header.bfType[0]!='B'||header.bfType[1]!='M',"invalid file type");

#define READ(what) fread(&what,1,sizeof(what),file);
		READ (header.bfSize);
		READ (header.bfReserved);
		READ (header.bfOffBits);
		READ (header.biSize);
		READ (header.biWidth);
		READ (header.biHeight);
		READ (header.biPlanes);
		READ (header.biBitCount);
		READ (header.biCompression);
		READ (header.biSizeImage);
		READ (header.biXPelsPerMeter);
		READ (header.biYPelsPerMeter);
		READ (header.biClrUsed);
		READ (header.biClrImportant);
#undef READ

		ERROR_CHECK (header.bfSize <= 0, "invalid image size");
		ERROR_CHECK (header.biWidth <= 0, "invalid image width");
		ERROR_CHECK (header.biHeight <= 0, "invalid image height");

		int bytesPerLine = (3 * (header.biWidth + 1) / 4) * 4;

		unsigned char *line = (unsigned char *) malloc (bytesPerLine);

		if (dataAlocated) {
			free (data);
		}
		data =
		    (unsigned char *) malloc (header.biWidth * header.biHeight * 3);
		ERROR_CHECK (line == NULL, "cannot allocate memory for line");
		ERROR_CHECK (data == NULL, "cannot allocate memory for data");
		dataAlocated = true;
		isRGBA = true;

		for (int i = header.biHeight - 1; i >= 0; i--) {
			int readed = fread (line, 1, bytesPerLine, file);

			for (int j = 0; j < header.biWidth; j++) {
				int ipos = 3 * (header.biWidth * i + j);

				data[ipos + 2] = line[3 * j];
				data[ipos + 1] = line[3 * j + 1];
				data[ipos] = line[3 * j + 2];

			}
			ERROR_CHECK (readed != bytesPerLine, "error while reading bytes");
		}
#undef ERROR_CHECK

		fclose (file);
		free (line);
		return 1;
	}
	bool save(char * filename) {

#define ERROR_CHECK(condition,message) \
 if(condition){\
  fprintf(stderr,"Error (%s) in file (%s) at line (%d)\n",message,__FILE__,__LINE__);\
  exit(1);\
  return false;\
 }
		ERROR_CHECK(data == NULL,"bitmap is emtpy");
		ERROR_CHECK (filename == NULL, "filename is null");

		FILE *file = fopen (filename, "w+");

		ERROR_CHECK (file == NULL, "cannot open file");
		fwrite (header.bfType, 1, 2, file);

#define WRITE(what) fwrite(&what,1,sizeof(what),file);
		WRITE(header.bfSize);
		WRITE (header.bfReserved);
		WRITE (header.bfOffBits);
		WRITE (header.biSize);
		WRITE (header.biWidth);
		WRITE (header.biHeight);
		WRITE (header.biPlanes);
		WRITE (header.biBitCount);
		WRITE (header.biCompression);
		WRITE (header.biSizeImage);
		WRITE (header.biXPelsPerMeter);
		WRITE (header.biYPelsPerMeter);
		WRITE (header.biClrUsed);
		WRITE (header.biClrImportant);
#undef WRITE

		int bytesPerLine = (3 * (header.biWidth + 1) / 4) * 4;
		char buffer[bytesPerLine];

		for (int i = header.biHeight - 1; i >= 0; i--) {
			for (int j = 0; j < header.biWidth; j++) {
				int ipos = 3*(header.biWidth * i + j);
				buffer[j*3+0]=data[ipos+2];
				buffer[j*3+1]=data[ipos+1];
				buffer[j*3+2]=data[ipos+0];
			}
			fwrite(buffer,1,bytesPerLine,file);
		}
		return true;
	}
	/** returns SIZExSIZE icon from bitmap **/
	Bmp* getIcon() {
#define SIZE 128
		ERROR_CHECK(data == NULL,"bitmap is emtpy");
		ERROR_CHECK(header.biWidth<SIZE,"image width is too small");
		ERROR_CHECK(header.biHeight<SIZE,"image height is too small");
		Bmp *ret=new Bmp();
		int bytesPerLine = (3 * (header.biWidth + 1) / 4) * 4;
		memcpy(&ret->header,&header,sizeof(header));
		ret->header.biWidth=SIZE;
		ret->header.biHeight=SIZE;
		ret->header.bfSize=SIZE*bytesPerLine+54;
		float ratiox=header.biWidth/SIZE;
		float ratioy=header.biHeight/SIZE;
		ret->data=(unsigned char*)malloc(SIZE*SIZE*3);
		ret->dataAlocated=true;
		for(int i=0; i<SIZE; i++)
			for(int j=0; j<SIZE; j++) {
				int x=round(ratiox*i);
				int y=round(ratioy*j);
				int outpos=3*(SIZE*i+j);
				int inpos=3*(header.biWidth*x+y);
				for(int k=0; k<3; k++)
					ret->data[outpos+k]=data[inpos+k];

			}
		//ret->print();
		return ret;
	}
};
#undef ERROR_CHECK
#endif
