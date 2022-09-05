#pragma once
#include "../program.hpp"
#include <vector>
#include <map>

class RendererComponent {
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::uvec3> indices;
	std::vector<glm::vec4> colors;
	bool indexed = false;
	GLenum mode = GL_POINTS;
	std::string textureName = "Default";
	GLuint texture;
public:
	RendererComponent();
	~RendererComponent();
	void setVertices(std::vector<glm::vec3>& vertices);
	std::vector<glm::vec3>& getVertices();
	void setIndices(std::vector<glm::uvec3>& indices);
	std::vector<glm::uvec3>& getIndices();
	void setColors(std::vector<glm::vec4>& colors);
	std::vector<glm::vec4>& getColors();
	void setColor(glm::vec4 color);
	glm::vec4& getColor();
	void setIndexed(bool useIndices);
	bool isIndexed();
	void setMode(GLenum mode);
	GLenum getMode();
	void setTexture(std::string textureName, GLuint texture);
	GLuint getTexture();
	std::string getTextureName();
};
