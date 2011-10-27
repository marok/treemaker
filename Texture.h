#ifndef _TEXTURE_H
#define _TEXTURE_H
#include "Bmp.h"

class Texture {

	Bmp bmp;


public:
	Texture() {
		isLoaded=false;
	}
	bool isLoaded;
	int width;
	int height;

	void load(char *path) {
		bmp.load(path);
		width=bmp.header.biWidth;
		height=bmp.header.biHeight;
	}
	/** makes 64x64 miniature from image **/
	void saveMiniature(char *name) {


	}


};

#endif