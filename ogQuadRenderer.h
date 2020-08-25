#pragma once

#include "libs.h"

#include "ogFontText.h"
#include "ogTexture.h"
#include "ogShader.h"

class ogQuadRenderer {

public:
	ogQuadRenderer();
	~ogQuadRenderer();

	void mSetTexture(ogTexture* aTexture);
	void mSetShader(ogShader* aShader);
	void mSetFontClass(ogFontClass* aFontClass);

	void mRender();
	void mRenderText(int x, int y, const char *simple_text, bool enable_kerning);

private:
	GLuint VAO, VBO, EBO;
	ogTexture* QuadTexture;
	ogShader* QuadShader;
	ogFontClass* cFontClass;
};
