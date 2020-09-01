#pragma once

#include "libs.h"

class ogTexture {

public:
	ogTexture();
	~ogTexture();

	int LoadTextureFromPngFile2(const char* aFilename);

	int LoadTextureFromPngFile(const char* aFilename, bool aFlip=false);

	GLuint getID();
	GLuint getWidth();
	GLuint getHeight();
	
	bool   mGetFlip();

	bool   hasAlpha();
	void   DestroyTexture();

private:
	// OpenGL Texture ID
	GLuint texID;

	// Texture Info
	GLuint width;
	GLuint height;
	bool alpha;
	bool flip;

	// PNG specific stuff
	png_byte colorType;
	png_byte colorDepth;
};
