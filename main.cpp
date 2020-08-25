#include <stdio.h>
#include <stdlib.h>

#include "AntTweakBar.h"
#include "libs.h"

#include "ogFontText.h"
#include "ogShader.h"
#include "ogTexture.h"
#include "ogQuadRenderer.h"

#define WINXBUILD
#ifdef WINXBUILD
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


//
// build and compile our shader program
//

#include "shader_source.h"	// built-in default shaders

int gFbWidth, gFbHeight;

void framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH) {
	glViewport(0, 0, fbW, fbH);
	gFbWidth  = fbW;
	gFbHeight = fbH;
}

void updateInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

// Callback function called by GLFW when window size changes
void WindowSizeCB(GLFWwindow *window, int width, int height)
{
	// Set OpenGL viewport and camera
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (double)width / height, 1, 10);
	gluLookAt(-1, 0, 3, 0, 0, 0, 0, 1, 0);

	// Send the new window size to AntTweakBar
	TwWindowSize(width, height);
}

void CursorPosCB(GLFWwindow* window, double x, double y) {
	TwEventMousePosGLFW(x, y);
}

// GLFWmousebuttonfun
void MouseButtonCB(GLFWwindow* window, int a, int b, int c) {
	TwEventMouseButtonGLFW(a, b);
}

void ScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
	// TwEventMouseWheelGLFW
	TwMouseWheel(yoffset);
}

// (GLFWkeyfun)TwEventKeyGLFW
void KeyCB(GLFWwindow* window, int key, int scancode, int action, int mods) {
	TwEventKeyGLFW(key, action);
}

void CharCB(GLFWwindow* window, unsigned int c) {	
	TwEventCharGLFW(c,0);
}

void R_drawBox(float x, float y, float w, float h, glm::vec4 color)
{
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor4f(color.x, color.y, color.z, color.w);

	glBegin(GL_QUADS);
	{
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
	}
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
}

void R_drawScreenDebug()
{
	glUseProgramObjectARB(NULL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int w = 16;
	int h = 16;
	glm::vec4 bgcolor(0.1f, 0.5f, 0.1f, 0.75f);
	for (int j = 0; j < gFbHeight; j += w) {
		for (int i = 0; i < gFbWidth; i += h) {
			R_drawBox(i, j, i+w, i+h, bgcolor);
	  	}
	}
}

void R_setClientOrthoMatrix(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);
}

void Enter2d(int w, int h) {

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void Exit2d() {
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}
//
// QUAD_CLASS
//
class RenderQuad {

public:
	RenderQuad() {

		this->VAO = 0;
		this->VBO = 0;
		this->EBO = 0;
		this->QuadTexture = NULL;
		this->QuadShader = NULL;
		this->cFontClass = NULL;
		
		// Static Quad Def.
		float vertices[] = {
			// positions          // colors           // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
		};
		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		// Initialize/Create VAO & VBO
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);				// Bind our VAO
#if 1
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);	// Bind our VBO
		glBufferData(GL_ARRAY_BUFFER, 1024 * sizeof(vertices), NULL, GL_DYNAMIC_DRAW);	// NOTE: NULL Buffer ptr as feed done by BufferSubData

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1024 * sizeof(indices), NULL, GL_DYNAMIC_DRAW); // TODO: Not expected to change use STATIC instead
#else
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#endif
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbind our VBO
		glBindVertexArray(0);				// Unbind our VAO
	};

	~RenderQuad() {
		if (!this->VAO)
			glDeleteVertexArrays(1, &this->VAO);
		if (!this->VBO)
			glDeleteBuffers(1, &this->VBO);
	};

	void SetTexture(ogTexture* aTexture) {
		QuadTexture = aTexture;
	};

	void SetShader(ogShader* aShader) {
		QuadShader = aShader;
	};

	void SetFontClass(ogFontClass* aFontClass) {
		this->cFontClass = aFontClass;
	}

	void Render() {


		// Simple Zoom factor
		float ax = (1.0f / gFbWidth) * this->QuadTexture->getWidth() * glm::cos(glfwGetTime()) * 4;
		float ay = (1.0f / gFbHeight) * this->QuadTexture->getHeight() * glm::cos(glfwGetTime()) * 4;

		//ax = (1.0f / 1920) * this->QuadTexture->getWidth() * 2;
		//ay = (1.0f / 1080) * this->QuadTexture->getHeight() * 2;

		// 2D basic translation
		float dx, dy;
		if (true) {
			dx = 0.5f * cos(glfwGetTime());
			dy = 0.5f * sin(glfwGetTime());
		}
		else {
			dx = 0;
			dy = 0;
		}

		ogCharDefinition* lcd = this->cFontClass->mLookupCharCD('A');
		
		float vertices[] = {
			// positions                    // colors                     // texture coords
			 0.5f * ax + dx,  0.5f * ay + dy, 0.0f,   1.0f, 0.0f, 0.0f,   lcd->tx1, lcd->ty1,   // top right
			 0.5f * ax + dx, -0.5f * ay + dy, 0.0f,   0.0f, 1.0f, 0.0f,   lcd->tx1, lcd->ty0,   // bottom right
			-0.5f * ax + dx, -0.5f * ay + dy, 0.0f,   0.0f, 0.0f, 1.0f,   lcd->tx0, lcd->ty0,   // bottom left
			-0.5f * ax + dx,  0.5f * ay + dy, 0.0f,   1.0f, 1.0f, 0.0f,   lcd->tx0, lcd->ty1	// top left 
		};

		float vertices2[] = {
			// positions                    // colors                     // texture coords
			 0.5f * ax + dx,  0.5f * ay + dy, 0.0f,   1.0f, 0.0f, 0.0f,   lcd->tx1, lcd->ty1,   // top right
			 0.5f * ax + dx, -0.5f * ay + dy, 0.0f,   0.0f, 1.0f, 0.0f,   lcd->tx1, lcd->ty0,   // bottom right
			-0.5f * ax + dx, -0.5f * ay + dy, 0.0f,   0.0f, 0.0f, 1.0f,   lcd->tx0, lcd->ty0,   // bottom left
			-0.5f * ax + dx,  0.5f * ay + dy, 0.0f,   1.0f, 1.0f, 0.0f,   lcd->tx0, lcd->ty1,	// top left 
			// positions                    // colors                     // texture coords
			 0.5f * ax ,  0.5f * ay, 0.0f,   1.0f, 0.0f, 0.0f,   lcd->tx1, lcd->ty1,   // top right
			 0.5f * ax , -0.5f * ay, 0.0f,   0.0f, 1.0f, 0.0f,   lcd->tx1, lcd->ty0,   // bottom right
			-0.5f * ax , -0.5f * ay, 0.0f,   0.0f, 0.0f, 1.0f,   lcd->tx0, lcd->ty0,   // bottom left
			-0.5f * ax ,  0.5f * ay, 0.0f,   1.0f, 1.0f, 0.0f,   lcd->tx0, lcd->ty1    // top left 
		};

		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3   // second triangle
		};

		unsigned int indices2[] = {
			0, 1, 3, // first triangle
			1, 2, 3, // second triangle

			4, 5, 7, // first triangle (e.g. prev Quad indices +4)
			5, 6, 7  // second triangle
		};

		// glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, QuadTexture->getID());

		// Set our VAO/VBO
		this->QuadShader->mBind();
		glBindVertexArray(this->VAO);				// Bind our VAO
		
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);	// Bind our VBO
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);	// Be sure to use glBufferSubData and not glBufferData
		glBindBuffer(GL_ARRAY_BUFFER, 0);			// Unbind our VBO

		glBindBuffer(GL_ARRAY_BUFFER, this->EBO);	// Bind our EBO
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(indices2), indices2);	// Be sure to use glBufferSubData and not glBufferData
		glBindBuffer(GL_ARRAY_BUFFER, 0);			// Unbind our EBO

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);	// Quads number to render * 6

		// Unbind our VAO
		glBindVertexArray(0);
		// Unbind Shader Prog.
		this->QuadShader->mUnBind();

		glBindTexture(GL_TEXTURE_2D, 0);
	};

private:
	GLuint VAO, VBO, EBO;
	ogTexture *QuadTexture;
	ogShader* QuadShader;
	ogFontClass* cFontClass;
};

std::string get_current_dir() {
	char buff[FILENAME_MAX]; //create string buffer to hold path
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
}


void build_font_atlas_offsets() {

	// 256 x 256 Atlas Font Texture of 16x16 Char

	float px, py;
	float ix = 2.0 / 256;	// texture_w
	float iy = 2.0 / 256;   // texture_h
	float offset[16][16][2];
	
	for (int y = 0, i=0; y < 256; y+=16, i++) {
		for (int x = 0, j=0; x < 256; x+=16, j++) {

			px = (x * ix) - 1;	// mapping in -1/ 1
			py = 1 - (y * iy);	// mapping in  1/-1
			offset[j][i][0] = px;
			offset[j][i][1] = py;
		}
	}
}

//
// MAIN()
//
int main(int argc, char** argv) {

	// Just a little helper to check path to dependencies
	std::cout << "Current working directory : " << get_current_dir() << std::endl;

	build_font_atlas_offsets();

	// Init GLFW
	glfwInit();

	// Create Window
	const int window_width = gFbWidth = 1920;
	const int window_height = gFbHeight = 1080;
	int framebufferWidth = 0;
	int framebufferHeight = 0;

	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	//  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MAC_OS

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenGLTuto", NULL, NULL);

	if (true) { // RESIZABLE_WINDOW
		glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	} else {
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		glViewport(0, 0, framebufferWidth, framebufferHeight);
	}

	glfwMakeContextCurrent(window); // Important for GLEW to initialize

	// Init GLEW (needs windows and opengl context)
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "ERROR::main.cpp:GLEW_INIT_FAILED" << "\n";
		glfwTerminate();
		exit(-1);
	}

#if 0
	// ANt
	{
		// Initialize AntTweakBar
		TwInit(TW_OPENGL, NULL);

		// Create a tweak bar
		TwBar* bar = TwNewBar("TweakBar");

		double time = 0, dt;// Current time and enlapsed time
		double turn = 0;    // Model turn counter
		double speed = 0.3; // Model rotation speed
		int wire = 0;       // Draw model in wireframe?
		float bgColor[] = { 0.1f, 0.2f, 0.4f };         // Background color 
		unsigned char cubeColor[] = { 255, 0, 0, 128 }; // Model color (32bits RGBA)

		TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.

		// Add 'speed' to 'bar': it is a modifable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [s] and [S].
		TwAddVarRW(bar, "speed", TW_TYPE_DOUBLE, &speed,
			" label='Rot speed' min=0 max=2 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");

		// Add 'wire' to 'bar': it is a modifable variable of type TW_TYPE_BOOL32 (32 bits boolean). Its key shortcut is [w].
		TwAddVarRW(bar, "wire", TW_TYPE_BOOL32, &wire,
			" label='Wireframe mode' key=w help='Toggle wireframe display mode.' ");

		// Add 'time' to 'bar': it is a read-only (RO) variable of type TW_TYPE_DOUBLE, with 1 precision digit
		TwAddVarRO(bar, "time", TW_TYPE_DOUBLE, &time, " label='Time' precision=1 help='Time (in seconds).' ");

		// Add 'bgColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR3F (3 floats color)
		TwAddVarRW(bar, "bgColor", TW_TYPE_COLOR3F, &bgColor, " label='Background color' ");

		// Add 'cubeColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR32 (32 bits color) with alpha
		TwAddVarRW(bar, "cubeColor", TW_TYPE_COLOR32, &cubeColor,
			" label='Cube color' alpha help='Color and transparency of the cube.' ");

		// Set GLFW event callbacks
		glfwSetWindowSizeCallback(window, WindowSizeCB);
		glfwSetMouseButtonCallback(window, MouseButtonCB);
		glfwSetCursorPosCallback(window, CursorPosCB);
		glfwSetScrollCallback(window, ScrollCB);
		glfwSetKeyCallback(window, KeyCB);
		glfwSetCharCallback(window, CharCB);


		TwWindowSize(window_width, window_height);
	}
#endif

	double zZoom = 1;
	float orbitDegrees = 0;
	//
	// Test Code -- for Shaders compile and link
	//

	// MAIN LOOP

	ogQuadRenderer*myQuad = new ogQuadRenderer();
	ogTexture *myTexture = new ogTexture();
	ogTexture* myTexture2 = new ogTexture();
	ogShader* myShader = new ogShader();

	//myTexture->LoadTextureFromPngFile("16x16_font.png");
	//myTexture->LoadTextureFromPngFile("container.png");
	ogFontClass* myFontClass = new ogFontClass("Bahnschrift2");
	ogFontClass* myFontClass2 = new ogFontClass("arabella");
	myTexture->LoadTextureFromPngFile("Fonts/Bahnschrift2.png");
	myTexture2->LoadTextureFromPngFile("Fonts/arabella.png");
	myShader->mLoadShadersFromFile("basic_texture_vertex.glsl", "basic_texture_frag.glsl");
	myQuad->mSetTexture(myTexture);
	myQuad->mSetShader(myShader);
	myQuad->mSetFontClass(myFontClass);

	//glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window)) {	

		// Polling events
		glfwPollEvents();

		// Check/Update inputs
		updateInput(window);

		// Clear		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		// ..
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (true) {
			int w, h;
			glfwGetWindowSize(window, &w, &h);
			glOrtho(-1, 1, -1, 1, -1, 1);
		}

		if (false) {
			gluLookAt(0, 0, 1, /* look from camera XYZ */
				0, 0, 0,     /* look at the origin */
				0, 1, 0);    /* positive Y up vector */
			glRotatef(orbitDegrees, 0.f, 0.f, 1.f);/*  the Z axis */
			orbitDegrees += 0.01;
		}
		
		myQuad->mRenderText(0,0, "The standard Lorem Ipsum passage, used since the 1500s", false);
		myQuad->mRenderText(0, -108, "The standard Lorem Ipsum passage, used since the 1500s", true);

		int w, h;
		glfwGetWindowSize(window, &w, &h);
		Enter2d(w, h);
		// Misc
		if (true) {
			GLint buffer_id;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer_id);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);			
			glfwGetWindowSize(window, &w, &h);
			R_setClientOrthoMatrix(w, h);
			R_drawScreenDebug();
			glBindFramebuffer(GL_FRAMEBUFFER, buffer_id);
		}
		Exit2d();
#if 0
		// Draw teaek bars
		TwDraw();
#endif
		// End Draw
		glfwSwapBuffers(window);
		glFlush();
	}

bail_me_out_now:;
	// Be nice and clean after ourselves
	delete myTexture;
	delete myQuad;
	delete myShader;

	// End/Exit Program
	glfwDestroyWindow(window);
	TwTerminate();
	glfwTerminate();

	return 0;
}