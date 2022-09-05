#include "shapeComponent.hpp"

/* Public */
ShapeComponent::ShapeComponent(ShapeName shapeName) {
	this->shapeName = shapeName;
	setMode(GL_TRIANGLES);
}

ShapeComponent::ShapeComponent(std::vector<glm::vec3>& vertices, std::vector<glm::vec4>& colors, GLenum mode) {
	setMode(mode);
	setVertices(vertices);
	setColors(colors);
}

ShapeComponent::ShapeComponent(std::vector<glm::vec3>& vertices, std::vector<glm::uvec3>& indices, std::vector<glm::vec4>& colors, GLenum mode) {
	setMode(mode);
	setVertices(vertices);
	setIndices(indices);
	setColors(colors);
}

ShapeComponent::~ShapeComponent() {}

void ShapeComponent::setPolygonName(ShapeName shapeName) {
	this->shapeName = shapeName;
}

ShapeName ShapeComponent::getShapeName() {
	return shapeName;
}

const char** ShapeComponent::getShapeNames() {
	const const char* shapeNames[2] = { "POLYGON", "RECTANGLE" };

	return shapeNames;
}
/* Public */
