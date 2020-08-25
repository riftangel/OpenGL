#pragma once

#include "libs.h"

struct ogCharDefinition {
	int id;					// The character id.
	int x, y;				// The left / Top position of the character image in the texture.
	int width, height;		// The width / height of the character image in the texture.
	float tx, ty;			// Texture coordinates from (0,0) top Left in [0,1]	-- helper 
	float tw, th;			// Texture width / height in (0,0)-(1-1) space      -- helper     
	float tx0, ty0;			// (0,0) bottom Left -- helper
	float tx1, ty1;			// (1,1) top right	 -- helper
	int xoffset, yoffset;	// How much the current position should be offset when copying the image from the texture to the screen.
	int xadvance;           // How much the current position should be advanced after drawing the character.  
	int page;				// The texture page where the character image is found.
	int chnnl;				// The texture channel where the character image is found (1 = blue, 2 = green, 4 = red, 8 = alpha, 15 = all channels).
};

class ogFontClass {

private:

	std::string cFontName;
	bool cIsLoaded;

	// Font info
	std::string cFace;	// This is the name of the true type font.
	int  cSize;			// The size of the true type font.
	bool cBold;			// The font is bold.
	bool cItalic;		// The font is italic.
	std::string cCharset;	// The name of the OEM charset used (when not unicode).
	bool cUnicode;		// Set to 1 if it is the unicode charset.
	int  cStretchH;		// The font height stretch in percentage. 100% means no stretch.
	bool cSmooth;		// Set to 1 if smoothing was turned on.
	int  cPadding[4];	// The padding for each character(up, right, down, left).
	int  cSpacing[2];	// The spacing for each character(horizontal, vertical).
	bool cAa;			// The supersampling level used. 1 means no supersampling was used.

	// Chars Map
	std::map<int, struct ogCharDefinition> cCharsMap;

	// Kerning Map
	// The kerning information is used to adjust the distance between certain characters, e.g.some characters should be placed closer to each other than others.
	//	first	The first character id.
	//	second	The second character id.
	//	amount	How much the x position should be adjusted when drawing the second character immediately following the first.
	std::map<int, std::map<int, int>> cKerningMap;

	// Font Common
	int cLineHeight;	// This is the distance in pixels between each line of text.
	int cBase;			// The number of pixels from the absolute top of the line to the base of the characters.
	int cScaleW;		// The width of the texture, normally used to scale the x pos of the character image.
	int cScaleH;		// The height of the texture, normally used to scale the y pos of the character image.
	int cPageCount;		// 	The number of texture pages included in the font. TODO: Only 1 page supported for now
	int cPacked;
	// TODO: Add support to Channels
	// packed	Set to 1 if the monochrome characters have been packed into each of the texture channels.In this case alphaChnl describes what is stored in each channel.
	// alphaChnl	Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyphand the outline, 3 if its set to zero, and 4 if its set to one.
	// redChnl	Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyphand the outline, 3 if its set to zero, and 4 if its set to one.
	// greenChnl	Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyphand the outline, 3 if its set to zero, and 4 if its set to one.
	// blueChnl	Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyphand the outline, 3 if its set to zero, and 4 if its set to one.


	// Font Page/Data(Image)
	std::string cFontImageFileName;
	int cPageId;	// TODO: Support multiple page

	int cCharsCount;

	// Misc.
	bool cDebug;

	// Private methods
	int mLoadFontDefinitionFile();

public:

	ogFontClass(const char* aFontName);
	~ogFontClass();

	bool mIsLoaded();

	int mGetScaleW();

	int mGetScaleH();

	ogCharDefinition* mLookupCharCD(const char aChar);

	int mGetKerning(int prev, int curr) {
		if (cKerningMap[prev].count(curr))
			return cKerningMap[prev][curr];
		else
			return 0;
	};

	int mGetSize() {
		return this->cSize;
	}
};
