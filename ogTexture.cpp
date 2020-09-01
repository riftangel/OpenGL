#pragma once

#include "ogTexture.h"

#include "SOIL2.h"

ogTexture::ogTexture() {

	this->texID      = 0;
	this->width      = 0;
	this->height     = 0;
	this->colorType  = 0;
	this->colorDepth = 0;
	this->alpha      = false;
	this->flip       = false;
};

ogTexture::~ogTexture() {
	this->DestroyTexture();
};

int ogTexture::LoadTextureFromPngFile2(const char* aFilename) {

	glGenTextures(1, &this->texID);
	glBindTexture(GL_TEXTURE_2D, this->texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* image = SOIL_load_image(
		aFilename,
		(int*)&this->width,
		(int*)&this->height,
		0,
		SOIL_LOAD_RGBA);

	if (!image) {
		std::cout << "png::Could not open the damn file.. please fix this and try again..." << std::endl;
		return -1;
	}

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		this->width,
		this->height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image);

	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(image);

	return 0;
}


int ogTexture::LoadTextureFromPngFile(const char* aFilename, bool flip) {

		this->flip = flip;

		png_bytep* rows = NULL, **data = &rows;

		char header[8];						// Mr. Header
		FILE* fp = fopen(aFilename, "rb");  // Miss File
		if (!fp) {
			std::cout << "png::Could not open the damn file.. please fix this and try again..." << std::endl;
			return -1;
		}
		// Check if our Mr. Header is right !
		fread(header, 1, 8, fp);
		if (png_sig_cmp((png_const_bytep)header, (png_size_t)0, (png_size_t)8)) {
			std::cout << "png::Something is fishy with this PNG header.. please fix this and try again..." << std::endl;
			fclose(fp);
			return -1;
		}
		// Initialize the struct libPNG uses for reading.
		png_structp pngReadStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!pngReadStruct) {
			std::cout << "png::Can't initialize - png_create_read_struct..." << std::endl;
			fclose(fp);
			return -1;
		}
		// More stuff to initialize... so easy ... as a walk in the park
		png_infop pngInfoStruct = png_create_info_struct(pngReadStruct);
		if (!pngInfoStruct) {
			std::cout << "png::Can't initialize - png_create_info..." << std::endl;
			fclose(fp);
			return -1;
		}
		// Really PNG Lib .. setjump ... Oh well we'll wait for you here in case of error
		if (setjmp(png_jmpbuf(pngReadStruct))) {
			std::cout << "png::Can't Read stuff - error during init_io ..." << std::endl;
			fclose(fp);
			return -1;
		}
		// Some other black PNG Lib magic ..
		png_init_io(pngReadStruct, fp);
		png_set_sig_bytes(pngReadStruct, 8);
		png_read_info(pngReadStruct, pngInfoStruct);

		// Fill the standard stuff
		this->width = png_get_image_width(pngReadStruct, pngInfoStruct);
		this->height = png_get_image_height(pngReadStruct, pngInfoStruct);
		this->colorType = png_get_color_type(pngReadStruct, pngInfoStruct);
		this->colorDepth = png_get_bit_depth(pngReadStruct, pngInfoStruct);
		this->alpha = this->colorType == PNG_COLOR_TYPE_RGBA;

		// Don't ask Why..
		png_set_interlace_handling(pngReadStruct);
		png_read_update_info(pngReadStruct, pngInfoStruct);

		// Read the damn image
		if (setjmp(png_jmpbuf(pngReadStruct))) {
			std::cout << "png::Can't Read stuff - error during read_image ..." << std::endl;
			fclose(fp);
			// Anything to free up...?
			if (*data) {
				for (unsigned int i = 0; i < this->height; ++i)
					if (rows[i]) free(rows[i]);	else break;
				free(*data);
			}
			return -1;
		}
		// Create space for the data.
		(*data) = (png_bytep*)malloc(sizeof(png_bytep) * this->height);
		memset(*data, 0, sizeof(png_bytep) * this->height);

		// Lovelly ... isn't it.. yet so simple
		for (int i = 0; i < (int)this->height; ++i)
			(*data)[i] = (png_byte*)malloc(png_get_rowbytes(pngReadStruct, pngInfoStruct));

		png_read_image(pngReadStruct, (*data));

		fclose(fp);

		// Now the fun stuff begin.. Create the OpenGL Texture ...
		bool isAlpha = (this->colorType == PNG_COLOR_TYPE_RGB) ? false : true;
		int lPadding = 3 + (isAlpha ? 1 : 0);
		size_t lAllocSize = sizeof(GLubyte) * this->width * this->height * lPadding;
		GLubyte* _Notnull_ texture_data = (GLubyte*)malloc(lAllocSize);
		memset(texture_data, 0, lAllocSize);
		int offset = isAlpha ? 4 : 3;
		if (!this->flip) {
			for (unsigned int i = 0; i < this->height; ++i)
			{
				for (unsigned int j = 0; j < this->width; ++j)
				{
					texture_data[i * this->width * offset + j * offset + 0] = rows[i][j * offset + 0];
					texture_data[i * this->width * offset + j * offset + 1] = rows[i][j * offset + 1];
					texture_data[i * this->width * offset + j * offset + 2] = rows[i][j * offset + 2];
					if (isAlpha) texture_data[i * this->width * offset + j * offset + 3] = rows[i][j * offset + 3];
				}
				free(rows[i]);	// No need for you anymore my little row
			}
			free(rows);
		}
		else {
			for (int i = (int)this->height - 1; i >= 0; i--)
			{
				int idx = this->height - (i + 1);
				for (unsigned int j = 0; j < this->width; ++j)
				{
					texture_data[idx * this->width * offset + j * offset + 0] = rows[i][j * offset + 0];
					texture_data[idx * this->width * offset + j * offset + 1] = rows[i][j * offset + 1];
					texture_data[idx * this->width * offset + j * offset + 2] = rows[i][j * offset + 2];
					if (isAlpha) texture_data[idx * this->width * offset + j * offset + 3] = rows[i][j * offset + 3];
				}
				free(rows[i]);	// No need for you anymore my little row
			}
			free(rows);
		}

		// Create OpenGL Texture
		glGenTextures(1, &this->texID);
		glBindTexture(GL_TEXTURE_2D, this->texID);
		// TODO: Should support differrent WRAP
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, true ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, true ? GL_LINEAR : GL_NEAREST);
		// Send the texture data to da GPU
		GLint type = isAlpha ? GL_RGBA : GL_RGB;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, type, this->width, this->height, 0, type, GL_UNSIGNED_BYTE, texture_data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		free(texture_data);

		return 0;	// Yeah !!!
	};

GLuint	ogTexture::getID() {
	return this->texID;
};

GLuint ogTexture::getWidth() {
	return this->width;
};

GLuint ogTexture::getHeight() {
	return this->height;
}
bool ogTexture::mGetFlip()
{
	return this->flip;
}
;

bool ogTexture::hasAlpha() {
	return this->alpha;
};

void ogTexture::DestroyTexture() {
	if (this->texID) {
		glDeleteTextures(1, &this->texID);
		this->texID = 0;
	}
};

