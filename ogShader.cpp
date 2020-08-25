#include "libs.h"

#include "ogShader.h"

GLint check_compile_link_status(GLuint aShaderId, const char* aShaderType, GLuint aMode) {

	GLint success;
	glGetShaderiv(aShaderId, aMode ? GL_LINK_STATUS : GL_COMPILE_STATUS, &success);
	if (!success)
	{
		int infoLength;
		glGetShaderiv(aShaderId, GL_INFO_LOG_LENGTH, &infoLength);
		GLchar* errorLog = (GLchar*)malloc(sizeof(GLchar) * infoLength);
		glGetShaderInfoLog(aShaderId, infoLength, NULL, errorLog);
		std::cout << "ERROR::SHADER::" << aShaderType << "::COMPILATION_FAILED\n" << errorLog << std::endl;
		free(errorLog);
	}
	return success;
}

GLuint build_shader_prog_from_source(const char* shader_source_vert, const char* shader_source_frag) {
	//
	// Compile Vertex Shader
	//
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &shader_source_vert, NULL);
	glCompileShader(vertexShader);
	//
	// Check for shader compile errors
	//
	if (!check_compile_link_status(vertexShader, "VERTEX_SHADER", 0)) {
		glDeleteShader(vertexShader);
		return 0;
	}
	//
	// Compile Fragment shader
	//
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &shader_source_frag, NULL);
	glCompileShader(fragmentShader);
	//
	// Check for shader compile errors
	//
	if (!check_compile_link_status(fragmentShader, "FRAGMENT_SHADER", 1)) {
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return 0;
	}
	//
	// Link shaders
	//
	GLuint shaderProgram = glCreateProgram();	// Return a non-zero value !
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//
	// Check for linking errors
	//
	if (!check_compile_link_status(shaderProgram, "PROGRAM_LINK", 1)) {
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderProgram);	// TODO: Check if this is required or not (Guess is not)
		return 0;
	}
	//
	// Delete Vertex/FragmenShaders 
	//
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

//
// ogShaderUniform
//
ogShaderUniform::ogShaderUniform() {
};

ogShaderUniform::~ogShaderUniform() {
};

//
// ogShader
//
ogShader::ogShader() {
	this->shaderId = 0;
};

ogShader::~ogShader() {
};

void ogShader::mBind() const {
	glUseProgram(this->shaderId);
};

void ogShader::mUnBind() const {
	glUseProgram(0);
};

void ogShader::mDispose() {
	if (shaderId) {
		glDeleteProgram(this->shaderId);
		this->shaderId = 0;
	}
};

GLuint ogShader::mLoadShadersFromText(const char* aVertexShaderSrc, const char* aFragShaderSrc) {
	GLuint shaderProg = build_shader_prog_from_source(aVertexShaderSrc, aFragShaderSrc);
	if (shaderProg)
		shaderId = shaderProg;
	return shaderId;
};

GLuint ogShader::mLoadShadersFromFile(const char* aVertexShaderFile, const char* aFragShaderFile) {
	std::ifstream fLoaderV;
	std::ifstream fLoaderF;
	fLoaderV.open(aVertexShaderFile, std::ios_base::in);
	if (!fLoaderV.is_open()) {
		std::cout << "Could not open VERTEX shader file : " << aVertexShaderFile << std::endl;
		return 0;
	}
	std::string shaderVertSrc(std::istreambuf_iterator<char>(fLoaderV), (std::istreambuf_iterator<char>()));
	fLoaderF.open(aFragShaderFile, std::ios_base::in);
	if (!fLoaderF.is_open()) {
		std::cout << "Could not open FRAG shader file : " << aFragShaderFile << std::endl;
		return 0;
	}
	std::string shaderFragSrc(std::istreambuf_iterator<char>(fLoaderF), (std::istreambuf_iterator<char>()));

	return mLoadShadersFromText(shaderVertSrc.c_str(), shaderFragSrc.c_str());
};
