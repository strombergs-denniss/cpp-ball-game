#pragma once
#include "rendererComponent.hpp"

enum ShapeName {
	POLYGON,
	RECTANGLE
};

class ShapeComponent : public RendererComponent {
private:
	ShapeName shapeName;
public:
	ShapeComponent(ShapeName shapeName = ShapeName::POLYGON);
	ShapeComponent(std::vector<glm::vec3>& vertices, std::vector<glm::vec4>& colors, GLenum mode = GL_TRIANGLES);
	ShapeComponent(std::vector<glm::vec3>& vertices, std::vector<glm::uvec3>& indices, std::vector<glm::vec4>& colors, GLenum mode = GL_TRIANGLES);
	~ShapeComponent();
	void setPolygonName(ShapeName shapeName);
	ShapeName getShapeName();
	static const char** getShapeNames();
};
