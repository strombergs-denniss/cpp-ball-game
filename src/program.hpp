#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/gl3w.h>
#include <iostream>
#include <string>

class Program {
private:
	std::string vertexShaderSource;
	std::string fragmentShaderSource;
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;
private:
	void compileShadersAndLinkProgram();
public:
	Program(std::string vertexShaderSource, std::string fragmentShaderSource);
	~Program();
	void use();
	void setAttrib(const GLchar* name, GLint size, GLenum type, GLboolean normalized, GLsizei stride);
	void setUniformVec4(const GLchar* name, glm::vec4 value);
	void setUniformMat4(const GLchar* name, glm::mat4 value);
	void setUniform1i(const GLchar* name, int value);
};
