#pragma once

#include "libs.h"

class ogCore {

public:
	ogCore();
	~ogCore();

	void mInit(int aWidth, int aHeight, int aOptions);
	int  mGetWindowWidth();
	int  mGetWindowHeight();
	GLFWwindow* mGetWindow();
	void mShutdownDown();
	void mSetFrameBufferCallback(GLFWframebuffersizefun aFrameBufferCallback);

private:
	int  m_Width;
	int  m_Height;
	int			m_Options;
	GLFWwindow* m_Window;
	void (*m_FrameBufferCallback_External)(GLFWwindow*, int, int);
	friend void gFrameBufferCallbackStatic(GLFWwindow* window, int fbW, int fbH);
};
