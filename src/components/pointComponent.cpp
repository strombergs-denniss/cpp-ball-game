#include "pointComponent.hpp"

/* Public */
PointComponent::PointComponent(glm::vec3 vertex, glm::vec4 color) {
	std::vector<glm::vec3>& vertices = getVertices();
	vertices.push_back(vertex);
	setColor(color);
	setMode(GL_POINTS);
}

PointComponent::PointComponent(std::vector<glm::vec3>& vertices, glm::vec4 color) {
	setVertices(vertices);
	setColor(color);
	setMode(GL_POINTS);
}

PointComponent::~PointComponent() {};
/* Public */
