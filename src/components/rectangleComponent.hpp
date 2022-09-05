#pragma once
#include "rendererComponent.hpp"

class RectangleComponent : public RendererComponent {
private:
	friend class GUI;
public:
	RectangleComponent(glm::vec4 color = glm::vec4(1.0f));
	~RectangleComponent();
};
