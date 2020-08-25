#pragma once

#include "libs.h"

class ogShaderUniform {

public:
	ogShaderUniform();
	~ogShaderUniform();
};

class ogShader {

private:
	GLuint shaderId;
	std::map<std::string, ogShaderUniform*> uniforms;

public:
	ogShader();
	~ogShader();

	void mBind() const;
	void mUnBind() const;
	void mDispose();

	GLuint mLoadShadersFromText(const char* aVertexShaderSrc, const char* aFragShaderSrc);
	GLuint mLoadShadersFromFile(const char* aVertexShaderFile, const char* aFragShaderFile);
};
