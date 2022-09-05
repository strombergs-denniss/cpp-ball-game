#include "program.hpp"

void Program::compileShadersAndLinkProgram() {
	const GLchar* vertexShaderSource = this->vertexShaderSource.c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	GLint isCompiled = 0;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);

	if (!isCompiled) {
		std::cout << "Vertex shader compilation error!\n";
		glDeleteShader(vertexShader);
		return;
	}

	const GLchar* fragmentShaderSource = this->fragmentShaderSource.c_str();

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);

	if (!isCompiled) {
		std::cout << "Fragment shader compilation error!\n";
		glDeleteShader(fragmentShader);
		return;
	}

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint isLinked = 0;

	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

	if (!isLinked) {
		std::cout << "Program linking error!\n";
		glDeleteProgram(program);
		return;
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Program::Program(std::string vertexShaderSource, std::string fragmentShaderSource) {
	this->vertexShaderSource = vertexShaderSource;
	this->fragmentShaderSource = fragmentShaderSource;
	compileShadersAndLinkProgram();
}

Program::~Program() {
	glDeleteProgram(program);
}

void Program::use() {
	glUseProgram(program);
}

void Program::setAttrib(const GLchar* name, GLint size, GLenum type, GLboolean normalized, GLsizei stride) {
	GLuint index = glGetAttribLocation(program, name);
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, normalized, stride, nullptr);
}

void Program::setUniformVec4(const GLchar* name, glm::vec4 value) {
	GLint location = glGetUniformLocation(program, name);
	glUniform4fv(location, 1, glm::value_ptr(value));
}

void Program::setUniformMat4(const GLchar* name, glm::mat4 value) {
	GLint location = glGetUniformLocation(program, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Program::setUniform1i(const GLchar* name, int value) {
	GLint location = glGetUniformLocation(program, name);
	glUniform1i(location, value);
}