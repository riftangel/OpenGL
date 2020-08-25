#pragma once

#include "libs.h"

class ogTexture {

public:
	ogTexture();
	~ogTexture();

	int LoadTextureFromPngFile(const char* aFilename);

	GLuint getID();
	GLuint getWidth();
	GLuint getHeight();

	bool   hasAlpha();
	void   DestroyTexture();

private:
	// OpenGL Texture ID
	GLuint texID;

	// Texture Info
	GLuint width;
	GLuint height;
	bool alpha;

	// PNG specific stuff
	png_byte colorType;
	png_byte colorDepth;
};
