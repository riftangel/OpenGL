#include "libs.h"

#include "ogQuadRenderer.h"

ogQuadRenderer::ogQuadRenderer() {

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

ogQuadRenderer::~ogQuadRenderer() {
	if (!this->VAO)
		glDeleteVertexArrays(1, &this->VAO);
	if (!this->VBO)
		glDeleteBuffers(1, &this->VBO);
};

void ogQuadRenderer::mSetTexture(ogTexture* aTexture) {
	QuadTexture = aTexture;
};

void ogQuadRenderer::mSetShader(ogShader* aShader) {
	QuadShader = aShader;
};

void ogQuadRenderer::mSetFontClass(ogFontClass* aFontClass) {
	this->cFontClass = aFontClass;
}

void ogQuadRenderer::mRender() {


	// Simple Zoom factor
	float ax = (1.0f / gFbWidth) * this->QuadTexture->getWidth() * (float)glm::cos(glfwGetTime()) * 4;
	float ay = (1.0f / gFbHeight) * this->QuadTexture->getHeight() * (float)glm::cos(glfwGetTime()) * 4;

	//ax = (1.0f / 1920) * this->QuadTexture->getWidth() * 2;
	//ay = (1.0f / 1080) * this->QuadTexture->getHeight() * 2;

	// 2D basic translation
	float dx, dy;
	if (true) {
		dx = 0.5f * (float)cos(glfwGetTime());
		dy = 0.5f * (float)sin(glfwGetTime());
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

void ogQuadRenderer::mRenderText(int x, int y, const char *simple_text, bool enable_kerning) {

	static bool gOnce = true;

	float vertices[(3 + 3 + 2) * 4 * 256];
	unsigned int indices[3 * 2 * 256];

	int index_vertice = 0;
	int index_indice  = 0;
	int count_draw_elt = 0;
	int sizeof_vertices = 0;
	int sizeof_indices =0;
	int prev_char = 0;
	int kerning = 0;
	int font_size = this->cFontClass->mGetSize();

	// Globals
	float x_scale = 1.0f / this->cFontClass->mGetScaleW();
	float y_scale = 1.0f / this->cFontClass->mGetScaleH();
	float x_pos = .0f + x;
	float y_pos = .0f + y;
	float x_aratio = (1.0f / gFbWidth);  // *this->QuadTexture->getWidth();	// TODO -- replace this with scale/translate/rotate Matrix
	float y_aratio = (1.0f / gFbHeight); // *this->QuadTexture->getHeight();

	int indice_delta = 0;
	
	for (int i = 0; i < strlen(simple_text); i++) {
		// Character info (NOT IN WORLD_SPACE coordinates !)
		ogCharDefinition* lcd = this->cFontClass->mLookupCharCD(simple_text[i]);

		int id_char = lcd->id;
		float c_width = lcd->width;
		float c_height = lcd->height;
		float x_offset = lcd->xoffset;	// TODO: This can be easily precomputed -- Look into doing this in the shader also.
		float y_offset = font_size - (lcd->yoffset+c_height);	// NOTE: MINUS !
		float x_advance = lcd->xadvance;
		float x0_texture = lcd->tx0;	// bottom left
		float y0_texture = lcd->ty0;
		float x1_texture = lcd->tx1;	// top right
		float y1_texture = lcd->ty0;

		if (gOnce) {
			std::cout << simple_text[i] << " id:" << id_char << " w:" << c_width << " h:" << c_height << " yo:" << y_offset << " " << c_height + y_offset <<std::endl;			
		}
		// Colors ... TBD
		// ...

		// Kerning
		if (prev_char && enable_kerning) {
			kerning = this->cFontClass->mGetKerning(prev_char, id_char);
			if (kerning) {
				//std::cout << "kerning: " << (char)prev_char << " " << (char)id_char << " amount " << kerning << std::endl;
			}
		}

		// Fill Vertice and Indices array
		float x_top_r = (x_pos + x_offset + c_width + kerning) * x_aratio;
		float y_top_r = (y_pos + y_offset + c_height) * y_aratio;
		float x_bot_r = x_top_r;
		float y_bot_r = (y_pos + y_offset) * y_aratio;
		float x_bot_l = (x_pos + x_offset + kerning) * x_aratio;
		float y_bot_l = y_bot_r;
		float x_top_l = x_bot_l;
		float y_top_l = y_top_r;

		float vertices_local[] = {
			// positions               // colors           // texture coords
			 x_top_r, y_top_r, 0.0f,   1.0f, 0.0f, 0.0f,   lcd->tx1, lcd->ty1,   // top right
			 x_bot_r, y_bot_r, 0.0f,   0.0f, 1.0f, 0.0f,   lcd->tx1, lcd->ty0,   // bottom right
			 x_bot_l, y_bot_r, 0.0f,   0.0f, 0.0f, 1.0f,   lcd->tx0, lcd->ty0,   // bottom left
			 x_top_l, y_top_l, 0.0f,   1.0f, 1.0f, 0.0f,   lcd->tx0, lcd->ty1	 // top left 
		};

		unsigned int indices_local[] = {
			0 + indice_delta, 1 + indice_delta, 3 + indice_delta, // first triangle
			1 + indice_delta, 2 + indice_delta, 3 + indice_delta  // second triangle
		};
		indice_delta += 4;

		memcpy(&vertices[index_vertice], vertices_local, sizeof(vertices_local));
		memcpy(&indices[index_indice], indices_local, sizeof(indices_local));
		index_vertice += 32;
		index_indice += 6;
		count_draw_elt += 6;
		sizeof_vertices += sizeof(vertices_local);
		sizeof_indices += sizeof(indices_local);

		x_pos += x_advance + kerning;
		prev_char = id_char;
	}
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, QuadTexture->getID());

	// Set our VAO/VBO
	this->QuadShader->mBind();
	glBindVertexArray(this->VAO);				// Bind our VAO

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);	// Bind our VBO
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof_vertices, vertices);	// Be sure to use glBufferSubData and not glBufferData
	glBindBuffer(GL_ARRAY_BUFFER, 0);			// Unbind our VBO

	glBindBuffer(GL_ARRAY_BUFFER, this->EBO);	// Bind our EBO
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof_indices, indices);	// Be sure to use glBufferSubData and not glBufferData
	glBindBuffer(GL_ARRAY_BUFFER, 0);			// Unbind our EBO

	glDrawElements(GL_TRIANGLES, count_draw_elt, GL_UNSIGNED_INT, 0);	// Quads number to render * 6

	// Unbind our VAO
	glBindVertexArray(0);
	// Unbind Shader Prog.
	this->QuadShader->mUnBind();
	// Unbind Texture
	glBindTexture(GL_TEXTURE_2D, 0);

	gOnce = false;
}
