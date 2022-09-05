#pragma once
#include "rendererComponent.hpp"

enum LineName { LINE_SEGMENT, POLYGONAL_CHAIN, CLOSED_POLYGONAL_CHAIN };

class LineComponent : public RendererComponent {
private:
	LineName lineName;
private:
	void deduceMode(LineName lineName);
public:
	LineComponent(glm::vec3 vertex0 = glm::vec3(1.0f), glm::vec3 vertex1 = glm::vec3(1.0f), glm::vec4 color = glm::vec4(1.0f));
	LineComponent(std::vector<glm::vec3> vertices, glm::vec4 color = glm::vec4(1.0f), LineName lineName = LineName::LINE_SEGMENT);
	~LineComponent();
	void setLineName(LineName lineName);
	LineName getLineName();
	static const char** getLineNames();
};
