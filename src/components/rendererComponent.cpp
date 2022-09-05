#include "rendererComponent.hpp"

/* Public */
RendererComponent::RendererComponent() {
	vertices.push_back(glm::vec3(0.0f));
	indices.push_back(glm::uvec3(0));
	colors.push_back(glm::vec4(1.0f));
}

RendererComponent::~RendererComponent() {}

void RendererComponent::setVertices(std::vector<glm::vec3>& vertices) {
	this->vertices = vertices;
}

std::vector<glm::vec3>& RendererComponent::getVertices() {
	return vertices;
}

void RendererComponent::setIndices(std::vector<glm::uvec3>& indices) {
	this->indices = indices;
}

std::vector<glm::uvec3>& RendererComponent::getIndices() {
	return indices;
}

void RendererComponent::setColors(std::vector<glm::vec4>& colors) {
	this->colors = colors;
}

std::vector<glm::vec4>& RendererComponent::getColors() {
	return colors;
}

void RendererComponent::setColor(glm::vec4 color) {
	colors[0] = color;
}

glm::vec4& RendererComponent::getColor() {
	return colors[0];
}

void RendererComponent::setIndexed(bool indexed) {
	this->indexed = indexed;
}

bool RendererComponent::isIndexed() {
	return indexed;
}

void RendererComponent::setMode(GLenum mode) {
	std::map<GLenum, bool> modes = {
		{GL_POINTS, true},
		{GL_LINES, true},
		{GL_LINE_STRIP, true},
		{GL_LINE_LOOP, true},
		{GL_TRIANGLES, true},
		{GL_TRIANGLE_STRIP, true},
		{GL_TRIANGLE_FAN, true}
	};

	if (modes[mode]) {
		this->mode = mode;
	}
}

GLenum RendererComponent::getMode() {
	return mode;
}

void RendererComponent::setTexture(std::string name, GLuint texture) {
	this->textureName = name;
	this->texture = texture;
}

GLuint RendererComponent::getTexture() {
	return texture;
}

std::string RendererComponent::getTextureName() {
	return textureName;
}

/* Public */
