#pragma once
#include "rendererComponent.hpp"

class PointComponent : public RendererComponent {
public:
	PointComponent(glm::vec3 vertex = glm::vec3(1.0f), glm::vec4 color = glm::vec4(1.0f));
	PointComponent(std::vector<glm::vec3>& vertices, glm::vec4 color = glm::vec4(1.0f));
	~PointComponent();
};
