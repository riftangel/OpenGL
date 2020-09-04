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
#include "ogCore.h"
#include "ogImgui.h"

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

class BasicQuad : public ogImguiRenderer {

public:
	BasicQuad() {

		this->VAO = 0;
		this->VBO = 0;
		this->EBO = 0;
		this->QuadTexture = NULL;
		this->QuadShader = NULL;
		this->m_color = glm::vec4(0.0f);
		this->m_factor = 0.10f;

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

	void renderer() {

		static int counter = 0;

		ImGui::Begin("Basic Quad Renderer");                    
		ImGui::Text("This is some useful text.");   


		ImGui::SliderFloat("Zoom Factor", &this->m_factor, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&m_color); 

		if (ImGui::Button("Button"))
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);
		ImGui::End();
	};

	void Render() {

		// Simple Zoom factor
		float ax = (1.0f / gFbWidth) * this->QuadTexture->getWidth() * glm::cos(glfwGetTime()) * 4;
		float ay = (1.0f / gFbHeight) * this->QuadTexture->getHeight() * glm::cos(glfwGetTime()) * 4;

		ax = (1.0f / 1920) * this->QuadTexture->getWidth() * this->m_factor * 10;
		ay = (1.0f / 1080) * this->QuadTexture->getHeight() * this->m_factor * 10;

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
	ogShader  *QuadShader;
	glm::vec4 m_color;
	float m_factor;
};

std::string get_current_dir() {
	char buff[FILENAME_MAX]; //create string buffer to hold path
	char *ptr = GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
}

class ogRenderToTexture : public ogImguiRenderer {

public:
	ogRenderToTexture() {

		this->VAO = 0;
		this->VBO = 0;
		this->EBO = 0;
		this->QuadTexture = NULL;
		this->QuadShader = NULL;
		this->m_color = glm::vec4(0.0f);
		this->m_factor = 0.0;

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
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);	// NOTE: NULL Buffer ptr as feed done by BufferSubData

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_DYNAMIC_DRAW); // TODO: Not expected to change use STATIC instead
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

	~ogRenderToTexture() {

		if (!this->VAO)
			glDeleteVertexArrays(1, &this->VAO);
		if (!this->VBO)
			glDeleteBuffers(1, &this->VBO);
	};

	bool mSetupTexture() {

		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
		GLuint FramebufferName = 0;
		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

		// The texture we're going to render to		
		glGenTextures(1, &this->renderedTexture);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, this->renderedTexture);

		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		// Poor filtering. Needed !
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// The depth buffer
		GLuint depthrenderbuffer;
		glGenRenderbuffers(1, &depthrenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

		// Set "renderedTexture" as our colour attachement #0
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->renderedTexture, 0);

		// Set the list of draw buffers.
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			return false;
		}

		// Render to our framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0, 0, 1024, 768); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		/*

		The fragment shader just needs a minor adaptation :

			layout(location = 0) out vec3 color;

		This means that when writing in the variable “color”, we will actually write in the Render Target 0, which happens to be our texture because DrawBuffers[0] is GL_COLOR_ATTACHMENTi, which is, in our case, renderedTexture.

		To recap :

		color will be written to the first buffer because of layout(location=0).
		The first buffer is GL_COLOR_ATTACHMENT0 because of DrawBuffers[1] = {GL_COLOR_ATTACHMENT0}
		GL_COLOR_ATTACHMENT0 has renderedTexture attached, so this is where your color is written.
		In other words, you can replace GL_COLOR_ATTACHMENT0 by GL_COLOR_ATTACHMENT2 and it will still work.

		Note : there is no layout(location=i) in OpenGL < 3.3, but you use glFragData[i] = mvvalue anyway.
		*/
		return true;
	};

	void renderer() {

		static int counter = 0;

		ImGui::Begin("Basic Renderer 2 Texture");
		ImGui::Text("This is some useful text.");


		ImGui::SliderFloat("Zoom Factor", &this->m_factor, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&m_color);

		if (ImGui::Button("Button"))
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);
		ImGui::End();
	};

	void mRender() {	
				
		float px = 1.0f - this->m_factor;
		float py = 1.0f - this->m_factor;

		// Static Quad Def.
		float vertices[] = {
			// positions          // colors           // texture coords
			 px,  py, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
			 px, -py, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
			-px, -py, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
			-px,  py, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
		};
		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		//glActiveTexture(GL_TEXTURE0);	// TODO: Check if this is still required with latest OpenGL 4.x
		glBindTexture(GL_TEXTURE_2D, QuadTexture->getID());

		// Set our VAO/VBO
		this->QuadShader->mBind();

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

		// Release Framebuffer !
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Restore view port
		glViewport(0, 0, gFbWidth, gFbHeight);

		// Draw on screen now...
		glBindTexture(GL_TEXTURE_2D, this->renderedTexture);

		// Set our VAO/VBO
		this->QuadShader->mBind();

		// DRAW QUAD !
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

	void SetTexture(ogTexture* aTexture) {
		QuadTexture = aTexture;
	};

	void SetShader(ogShader* aShader) {
		QuadShader = aShader;
	};

private:
	GLuint renderedTexture;
	GLuint VAO, VBO, EBO;
	ogTexture* QuadTexture;
	ogShader* QuadShader;
	glm::vec4 m_color;
	float m_factor;
};

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
	gCore->mSetFrameBufferCallback(framebuffer_resize_callback);

	ogImgui *gImgui = new ogImgui(gCore->mGetWindow());
	
	double zZoom = 1;
	float orbitDegrees = 0;

	//
	// INIT Shaders, Texture and Fonts...
	//
	ogQuadRenderer*myQuad = new ogQuadRenderer();
	ogTexture *myTexture = new ogTexture();
	ogTexture* myTexture2 = new ogTexture();
	ogTexture* myTexture3 = new ogTexture();

	ogShader* myShader = new ogShader();
	ogShader* myShaderText = new ogShader();
	ogShader* myShaderT2B = new ogShader();

	//myTexture->LoadTextureFromPngFile("16x16_font.png");
	//myTexture->LoadTextureFromPngFile("container.png");
	ogFontClass* myFontClass = new ogFontClass("Bahnschrift2");
	ogFontClass* myFontClass2 = new ogFontClass("Candara2");
	myTexture->LoadTextureFromPngFile("Fonts/Bahnschrift2.png", true);
	myTexture2->LoadTextureFromPngFile("Fonts/Candara2.png", true);
	myTexture3->LoadTextureFromPngFile("Assets/basic1024_768.png", true);
	myShader->mLoadShadersFromFile("Shaders/basic_texture_vertex.glsl", "Shaders/basic_texture_frag.glsl");
	myShaderText->mLoadShadersFromFile("Shaders/text_texture_vertex.glsl", "Shaders/text_texture_frag.glsl");
	myShaderT2B->mLoadShadersFromFile("Shaders/render2texture_vertex.glsl", "Shaders/render2texture_frag.glsl");

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

	ogRenderToTexture *myRender2Tex = new ogRenderToTexture();
	myRender2Tex->SetShader(myShaderT2B);
	myRender2Tex->SetTexture(myTexture3);
	myRender2Tex->mSetupTexture();
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

		myRender2Tex->mRender();		

		gImgui->mPushCallback(myBasicQuad);
		gImgui->mPushCallback(myRender2Tex);
		gImgui->mRender();

		// End Draw
		glfwSwapBuffers(gCore->mGetWindow());
		glFlush();
	}

bail_me_out_now:;
	// Be nice and clean after ourselves
	delete myTexture;
	delete myQuad;
	delete myShader;

	// End Imgui 
	delete gImgui;

	// End/Exit Program
	gCore->mShutdownDown();

	return 0;
}