#include "ogFontText.h"

#include <fstream>
#include <sstream>
#include <map>

int ogFontClass::mLoadFontDefinitionFile()
{
	std::string lFontDefinitionFileName = "Fonts/" + cFontName + ".fnt";
	std::string lLine;
	std::string lToken;
	int lLineNumber = 0;
	std::ifstream lFileLoader;

	lFileLoader.open(lFontDefinitionFileName);
	if (!lFileLoader.is_open()) {
		std::cerr << "Could not open font file definition : " << lFontDefinitionFileName << std::endl;
		return -1;
	}
	// Parse INFO
	std::map<std::string, std::string> lInfoMap;
	while (std::getline(lFileLoader,lLine)) {
		
		if (this->cDebug) {
			std::cout << lLineNumber << " : " << lLine << std::endl;
		}

		int lPos, lSep;
		std::string lKey, lValue;
		while ((lPos = (int)lLine.find(' ')) != std::string::npos || (lPos = (int)lLine.length())) {
			
			lToken = lLine.substr(0, lPos);
			lSep = (int)lToken.find('=');
			lKey = lToken.substr(0, (const size_t)lSep);
			if (lSep == std::string::npos)
				lValue = "";
			else
				lValue = lToken.substr((size_t)lSep+1, lToken.length() - lSep);
			lInfoMap[lKey] = lValue;
			if (this->cDebug) std::cout << lKey << " : " << lValue << std::endl;
			lLine.erase(0, (size_t)lPos + 1);
		}
		lLineNumber++;
		if (lLineNumber == 4)
			break;
	}
	
	// INFO initialization...
	this->cFace = lInfoMap["face"];
	this->cSize = atoi(lInfoMap["size"].c_str());
	this->cBold = atoi(lInfoMap["bold"].c_str()) ? true : false;
	this->cItalic = atoi(lInfoMap["italic"].c_str()) ? true : false;
	this->cCharset = lInfoMap["charset"];
	this->cUnicode = atoi(lInfoMap["unicode"].c_str()) ? true : false;
	this->cStretchH = atoi(lInfoMap[" stretchH"].c_str());
	this->cSmooth = atoi(lInfoMap["size"].c_str()) ? true : false;
	this->cAa = atoi(lInfoMap["aa"].c_str()) ? true : false;
	std::string lPadding = lInfoMap["padding"];
	int lIdx = 0, lPos;
	while ((lPos = (int)lPadding.find(',')) != std::string::npos || (lPos = (int)lPadding.length())) {
		lToken = lPadding.substr(0, lPos);
		this->cPadding[lIdx++] = atoi(lToken.c_str());
		lPadding.erase(0, (size_t)lPos + 1);
	}
	std::string lSpacing = lInfoMap["spacing"];
	lIdx = 0;
	while ((lPos = (int)lSpacing.find(',')) != std::string::npos || (lPos = (int)lSpacing.length())) {
		lToken = lSpacing.substr(0, lPos);
		this->cSpacing[lIdx] = atoi(lToken.c_str());
		lSpacing.erase(0, (size_t)lPos + 1);
		lIdx++;
	}
	// Font Common
	this->cLineHeight = atoi(lInfoMap["lineHeight"].c_str());
	this->cBase = atoi(lInfoMap["base"].c_str());
	this->cScaleW = atoi(lInfoMap["scaleW"].c_str());
	this->cScaleH = atoi(lInfoMap["scaleH"].c_str());
	this->cPageCount = atoi(lInfoMap["pages"].c_str());
	this->cPacked = atoi(lInfoMap["packed"].c_str()) ? true : false;
	// Font Page
	this->cFontImageFileName = "Fonts/" + lInfoMap["file"];
	this->cPageId = atoi(lInfoMap["id"].c_str());

	// Parse Characters
	int lKerningsCount = 0;
	float ix = 1.0f / this->cScaleW;
	float iy = 1.0f / this->cScaleH;
	while (std::getline(lFileLoader, lLine)) {

		if (this->cDebug) {
			std::cout << lLineNumber << " : " << lLine << std::endl;
		}

		int lPos, lSep;
		std::string lKey, lValue;
		std::map<std::string, int> lCharDefinition;
		ogCharDefinition lCD;
		while ((lPos = (int)lLine.find(' ')) != std::string::npos || (lPos = (int)lLine.length())) {

			lToken = lLine.substr(0, lPos);
			lSep = (int)lToken.find('=');
			lKey = lToken.substr(0, (const size_t)lSep);
			if (lSep == std::string::npos)
				lValue = "";
			else
				lValue = lToken.substr((size_t)lSep + 1, lToken.length() - lSep);
			if (lKey == "count") {
				lKerningsCount = atoi(lValue.c_str());
				break;
			}
			if (lKey != "" && lValue != "") {
				lCharDefinition[lKey] = atoi(lValue.c_str());
				if (this->cDebug) std::cout << lKey << " : " << lValue << std::endl;
			}
			lLine.erase(0, (size_t)lPos + 1);
		}
		lCD.id      = lCharDefinition["id"];
		lCD.x       = lCharDefinition["x"];
		lCD.y       = lCharDefinition["y"];
		lCD.tx      = lCD.x * ix;
		lCD.ty      = lCD.y * iy;
		lCD.width   = lCharDefinition["width"];
		lCD.height  = lCharDefinition["height"];
		lCD.tw      = lCD.width  * ix;
		lCD.th      = lCD.height * iy;
		lCD.tx0     = lCD.tx;
		lCD.ty0     = 1.0f - (lCD.ty + lCD.th);
		lCD.tx1     = lCD.tx + lCD.tw;
		lCD.ty1      = 1.0f - lCD.ty;
		lCD.xoffset = lCharDefinition["xoffset"];
		lCD.yoffset = lCharDefinition["yoffset"];
		lCD.xadvance = lCharDefinition["xadvance"];
		lCD.page     = lCharDefinition["page"];
		lCD.chnnl    = lCharDefinition["chnnl"];
		this->cCharsMap[lCD.id] = lCD;
		lLineNumber++;
		if (lKerningsCount)
			break;
	} 
	// Parse Kerning
	while (std::getline(lFileLoader, lLine)) {

		if (this->cDebug) {
			std::cout << lLineNumber << " : " << lLine << std::endl;
		}

		int lPos, lSep;
		std::string lKey, lValue;		
		std::map<std::string, std::string> lKerningEntry;
		while ((lPos = (int)lLine.find(' ')) != std::string::npos || (lPos = (int)lLine.length())) {

			lToken = lLine.substr(0, lPos);
			lSep = (int)lToken.find('=');
			lKey = lToken.substr(0, (const size_t)lSep);
			lValue = lToken.substr((size_t)lSep + 1, lToken.length() - lSep);
			if (lSep != std::string::npos) {
				lKerningEntry[lKey] = lValue;
				if (this->cDebug) std::cout << lKey << " : " << lValue << std::endl;
			}
			lLine.erase(0, (size_t)lPos + 1);
		}
		int lSecond = atoi(lKerningEntry["second"].c_str());
		int lAmount = atoi(lKerningEntry["amount"].c_str());
		this->cKerningMap[atoi(lKerningEntry["first"].c_str())][lSecond] = lAmount;
		lLineNumber++;
	}

 	lFileLoader.close();
	return 0;
}

ogFontClass::ogFontClass(const char* aFontName) {

	this->cFontName = aFontName;
	this->cDebug = false;

	this->mLoadFontDefinitionFile();

}

ogFontClass::~ogFontClass() {

}

bool ogFontClass::mIsLoaded() {
	return this->cIsLoaded;
}

int ogFontClass::mGetScaleW() {
	return this->cScaleW;
}

int ogFontClass::mGetScaleH() {
	return this->cScaleH;
}

ogCharDefinition *ogFontClass::mLookupCharCD(const char aChar) {

	return this->cCharsMap.count((int)aChar) ? &this->cCharsMap[(int)aChar] : NULL;
}

