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

	void    mSetScaleFactor(GLfloat aScaleFactor);
	GLfloat mGetScaleFactor();

	void mRender();
	void mRenderText(int x, int y, const char *simple_text, bool enable_kerning);

private:
	GLuint       m_VAO, m_VBO, m_EBO;
	ogTexture*   m_QuadTexture;
	ogShader*    m_QuadShader;
	ogFontClass* m_FontClass;
	GLfloat      m_ScaleFactor;
};
