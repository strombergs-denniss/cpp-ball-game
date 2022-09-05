#include "lineComponent.hpp"

/* Private */
void LineComponent::deduceMode(LineName lineName) {
	GLenum lineNames[] = { GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP };
	setMode(lineNames[lineName]);
}
/* Private */

/* Public */
LineComponent::LineComponent(glm::vec3 vertex0, glm::vec3 vertex1, glm::vec4 color) {
	setLineName(LineName::LINE_SEGMENT);
	std::vector<glm::vec3>& vertices = getVertices();
	vertices.push_back(vertex0);
	vertices.push_back(vertex1);
	setColor(color);
}

LineComponent::LineComponent(std::vector<glm::vec3> vertices, glm::vec4 color, LineName lineName) {
	setLineName(lineName);
	setVertices(vertices);
	setColor(color);
}

LineComponent::~LineComponent() {}

void LineComponent::setLineName(LineName lineName) {
	this->lineName = lineName;
	deduceMode(lineName);
}

LineName LineComponent::getLineName() {
	return lineName;
}

const char** LineComponent::getLineNames() {
	const char* lineNames[3] = { "LINE_SEGMENT", "POLYGONAL_CHAIN", "CLOSED_POLYGONAL_CHAIN" };

	return lineNames;
}
/* Public */
