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
	std::map<std::string, GLint> m_UniformsLocation;
public:
	ogShader();
	~ogShader();

	void mBind() const;
	void mUnBind() const;
	void mDispose();

	GLuint mLoadShadersFromText(const char* aVertexShaderSrc, const char* aFragShaderSrc);
	GLuint mLoadShadersFromFile(const char* aVertexShaderFile, const char* aFragShaderFile);

	GLint mGetUniformLocation(const char* aUniformName);

	void mSetUniformValue(const char* aUniformLocation, GLint aValue);
	void mSetUniformValue(const char* aUniformLocation, float aValue);
	void mSetUniformValue(const char* aUniformLocation, glm::fvec2 aVector2);
	void mSetUniformValue(const char* aUniformLocaiton, glm::fvec3 aVector3);
	void mSetUniformValue(const char* aUniformLocation, glm::fvec4 aVector4);
	void mSetUniformValue(const char* aUniformName, glm::mat3 aMat3, GLboolean aTranspose = GL_FALSE);
	void mSetUniformValue(const char* aUniformName, glm::mat4 aMat4, GLboolean aTranspose = GL_FALSE);
};
