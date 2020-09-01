#include <stdio.h>
#include <stdlib.h>

#include "AntTweakBar.h"
#include "libs.h"

#include "ogFontText.h"
#include "ogShader.h"
#include "ogTexture.h"
#include "ogQuadRenderer.h"
#include "ogInputManager.h"
#include "ogToolbox.h"

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

// Change this to gCore getWidth/getHeight ...
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

void setup2DGrid(GLFWwindow *aWindow) {

	int w, h;
	glfwGetWindowSize(aWindow, &w, &h);
	Enter2d(w, h);
	GLint buffer_id;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glfwGetWindowSize(aWindow, &w, &h);
	R_setClientOrthoMatrix(w, h);
	R_drawScreenDebug();
	glBindFramebuffer(GL_FRAMEBUFFER, buffer_id);
	Exit2d();
}

//
// QUAD_CLASS
//
class BasicQuad {

public:
	BasicQuad() {

		this->VAO = 0;
		this->VBO = 0;
		this->EBO = 0;
		this->QuadTexture = NULL;
		this->QuadShader = NULL;
		
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
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);	// Bind our VBO
		glBufferData(GL_ARRAY_BUFFER, 1024 * sizeof(vertices), NULL, GL_DYNAMIC_DRAW);	// NOTE: NULL Buffer ptr as feed done by BufferSubData

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1024 * sizeof(indices), NULL, GL_DYNAMIC_DRAW); // TODO: Not expected to change use STATIC instead
		// Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(void*)0);
		glEnableVertexAttribArray(0);
		// Colors
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// Texture Coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbind our VBO
		glBindVertexArray(0);				// Unbind our VAO
	};

	~BasicQuad() {
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

	void Render() {

		// Simple Zoom factor
		float ax = (1.0f / gFbWidth) * this->QuadTexture->getWidth() * glm::cos(glfwGetTime()) * 4;
		float ay = (1.0f / gFbHeight) * this->QuadTexture->getHeight() * glm::cos(glfwGetTime()) * 4;

		ax = (1.0f / 1920) * this->QuadTexture->getWidth() * 1;
		ay = (1.0f / 1080) * this->QuadTexture->getHeight() * 1;

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
		
		struct slcd {
			float tx0, ty0, tx1, ty1;
		};

		slcd pcd = { 0, 0, 1, 1 };
		slcd* lcd = &pcd;

		float vertices[] = {
			// positions          // colors           // texture coords
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   lcd->tx0, lcd->ty1,	// top left 
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   lcd->tx1, lcd->ty1,   // top right
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   lcd->tx1, lcd->ty0,   // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   lcd->tx0, lcd->ty0,   // bottom left
		};

		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3   // second triangle
		};

		glActiveTexture(GL_TEXTURE0);	// TODO: Check if this is still required with latest OpenGL 4.x
		glBindTexture(GL_TEXTURE_2D, QuadTexture->getID());

		// Set our VAO/VBO
		this->QuadShader->mBind();

#if 1
		glm::vec3 translation(-dx, dy, 0.f);
		glm::vec3 rotation(0.0f, 0.0f, 0.0f);
		glm::vec3 scale(ax, ay, 1.0f);
		glm::mat4 trans = fnCreateTransformationMatrix(translation, rotation, scale);
#else
		glm::mat4 trans(1.0f);
#endif
		this->QuadShader->mSetUniformValue("transformation_mat", trans);

		glBindVertexArray(this->VAO);				// Bind our VAO
		
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);	// Bind our VBO
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
		glBindBuffer(GL_ARRAY_BUFFER, 0);			// Unbind our VBO

		glBindBuffer(GL_ARRAY_BUFFER, this->EBO);	// Bind our EBO
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(indices), indices);	 // Be sure to use glBufferSubData and not glBufferData
		glBindBuffer(GL_ARRAY_BUFFER, 0);			// Unbind our EBO

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	// Quads number to render * 6

		// Unbind our VAO
		glBindVertexArray(0);
		// Unbind Shader Prog.
		this->QuadShader->mUnBind();

		glBindTexture(GL_TEXTURE_2D, 0);
	};

private:
	GLuint VAO, VBO, EBO;
	ogTexture *QuadTexture;
	ogShader  *QuadShader;;
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

class ogCore {

public:
	ogCore() {
		this->m_Width  = -1;
		this->m_Height = -1;
		this->m_Options = 0;
		this->m_FrameBufferCallback_External = NULL;
		this->m_Window = NULL;
	};
	
	~ogCore() {

	};

	void mInit(int aWidth, int aHeight, int aOptions) {

		// Init GLFW
		glfwInit();

		// Initialize Core internals
		this->m_Width   = aWidth;
		this->m_Height  = aHeight;
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

		glfwSetFramebufferSizeCallback(this->m_Window, framebuffer_resize_callback);

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

	void mFrameBufferCallback(GLFWwindow* window, int fbW, int fbH) {
		
		glViewport(0, 0, fbW, fbH);
		this->m_Width  = fbW;
		this->m_Height = fbH;
		
		if (m_FrameBufferCallback_External) {
			this->mFrameBufferCallback(window, fbW, fbH);
		}
	}

	int  mGetWindowWidth() {
		return this->m_Width;
	};

	int  mGetWindowHeight() {
		return this->m_Height;
	}

	GLFWwindow* mGetWindow() {
		return this->m_Window;
	}

	void mShutdownDown() {

		glfwDestroyWindow(this->m_Window);
		glfwTerminate();
	};

private:
	int         m_Width;
	int         m_Height;
	int			m_Options;
	GLFWwindow *m_Window;
	void		(*m_FrameBufferCallback_External)(GLFWwindow*, int, int);
};


ogCore* gCore = NULL;

extern int main_test();
//
// MAIN()
//
int main(int argc, char** argv) {

	// Just a little helper to check path to dependencies
	std::cout << "Current working directory : " << get_current_dir() << std::endl;

	// main_test();
	gCore = new ogCore();
	gCore->mInit(1920,1080,0);

	double zZoom = 1;
	float orbitDegrees = 0;

	//
	// INIT Shaders, Texture and Fonts...
	//
	ogQuadRenderer*myQuad = new ogQuadRenderer();
	ogTexture *myTexture = new ogTexture();
	ogTexture* myTexture2 = new ogTexture();
	ogShader* myShader = new ogShader();
	ogShader* myShaderText = new ogShader();

	//myTexture->LoadTextureFromPngFile("16x16_font.png");
	//myTexture->LoadTextureFromPngFile("container.png");
	ogFontClass* myFontClass = new ogFontClass("Bahnschrift2");
	ogFontClass* myFontClass2 = new ogFontClass("Candara2");
	myTexture->LoadTextureFromPngFile("Fonts/Bahnschrift2.png", true);
	myTexture2->LoadTextureFromPngFile("Fonts/Candara2.png", true);
	myShader->mLoadShadersFromFile("Shaders/basic_texture_vertex.glsl", "Shaders/basic_texture_frag.glsl");
	myShaderText->mLoadShadersFromFile("Shaders/text_texture_vertex.glsl", "Shaders/text_texture_frag.glsl");

	myQuad->mSetTexture(myTexture);
	myQuad->mSetShader(myShaderText);
	myQuad->mSetFontClass(myFontClass);

	//
	// Set GL_CAPABILITIES...
	//
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_SCISSOR_TEST);

	ogInputManager* myInputMgr = new ogInputManager();
	myInputMgr->mRegisterWindowCallback(gCore->mGetWindow());

	BasicQuad* myBasicQuad = new BasicQuad();
	myBasicQuad->SetShader(myShader);
	myBasicQuad->SetTexture(myTexture);

	//
	// MAIN_LOOP
	//
	while (!glfwWindowShouldClose(gCore->mGetWindow())) {	

		// Polling events
		glfwPollEvents();

		// Check/Update inputs
		updateInput(gCore->mGetWindow());
		std::map<int, cb_data_info> keymap = myInputMgr->mGetKeyMap();

		// Clear		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		// Grid / Debug and other 2D stuff
		setup2DGrid(gCore->mGetWindow());
#if 0
		myQuad->mSetScaleFactor(1.0f + myInputMgr->mGetScrollFactor() * 20.0f);

		myQuad->mSetTexture(myTexture2);
		myQuad->mSetFontClass(myFontClass2);
		myQuad->mRenderText(-900,0, "The standard Lorem Ipsum passage, used since the 1500s", false);

		myQuad->mSetTexture(myTexture2);
		myQuad->mSetFontClass(myFontClass2);
		myQuad->mRenderText(-900, -108, "The standard Lorem Ipsum passage, used since the 1500s", true);
		
		// Mouse Coordinates
		char mouse_pos_text[48];
		double mouse_x, mouse_y;
		myInputMgr->mGetMousePos(mouse_x, mouse_y);
		snprintf(mouse_pos_text, sizeof(mouse_pos_text), "m_posx: %d m_posy: %d", (int)mouse_x, (int)mouse_y);
		myQuad->mRenderText(-900, -216, mouse_pos_text, true);

		myInputMgr->mGetScrollPos(mouse_x, mouse_y);
		snprintf(mouse_pos_text, sizeof(mouse_pos_text), "m_scrollx: %d m_scrolly: %d factor: %f", (int)mouse_x, (int)mouse_y, myInputMgr->mGetScrollFactor());
		myQuad->mRenderText(-900, -324, mouse_pos_text, true);


		myQuad->mSetScaleFactor(1.0f);

		// Simple Quad ?
		// myQuad->mRender();
#endif
		myBasicQuad->Render();

		// End Draw
		glfwSwapBuffers(gCore->mGetWindow());
		glFlush();
	}

bail_me_out_now:;
	// Be nice and clean after ourselves
	delete myTexture;
	delete myQuad;
	delete myShader;

	// End/Exit Program
	gCore->mShutdownDown();

	return 0;
}