#include "ogCore.h"


ogCore* global_core = NULL;

void gFrameBufferCallbackStatic(GLFWwindow* window, int fbW, int fbH) {
	glViewport(0, 0, fbW, fbH);
	global_core->m_Width  = fbW;
	global_core->m_Height = fbH;
	global_core->m_FrameBufferCallback_External(window, fbW, fbH);
}

ogCore::ogCore() {
	this->m_Width = -1;
	this->m_Height = -1;
	this->m_Options = 0;
	this->m_FrameBufferCallback_External = NULL;
	this->m_Window = NULL;
};

ogCore::~ogCore() {
};

void ogCore::mInit(int aWidth, int aHeight, int aOptions) {

	// Init GLFW
	glfwInit();

	// TODO: Remove hack around callback
	assert(global_core == NULL);
	global_core = this;

	// Initialize Core internals
	this->m_Width = aWidth;
	this->m_Height = aHeight;
	this->m_Options = aOptions;

	// Temporary workaround.. TODO: Switch to getWidth/Height
	gFbWidth = this->m_Width;
	gFbHeight = this->m_Height;

	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	//  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MAC_OS

	this->m_Window = glfwCreateWindow(this->m_Width, this->m_Height, "ogCoreOpenGL", NULL, NULL);

	glfwSetFramebufferSizeCallback(this->m_Window, (GLFWframebuffersizefun)gFrameBufferCallbackStatic);

	glViewport(0, 0, this->m_Width, this->m_Height);

	glfwMakeContextCurrent(this->m_Window); // Important for GLEW to initialize

	// Init GLEW (needs windows and opengl context)
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "CRITICAL_ERROR: ogCore::mInit() : GLEW_INIT_FAILED" << "\n";
		glfwTerminate();
		exit(-1);
	}
};

void ogCore::mSetFrameBufferCallback(GLFWframebuffersizefun aFrameBufferCallback) {

	this->m_FrameBufferCallback_External = aFrameBufferCallback;
}

int  ogCore::mGetWindowWidth() {
	return this->m_Width;
};

int  ogCore::mGetWindowHeight() {
	return this->m_Height;
}

GLFWwindow* ogCore::mGetWindow() {
	return this->m_Window;
}

void ogCore::mShutdownDown() {

	glfwDestroyWindow(this->m_Window);
	glfwTerminate();
};
