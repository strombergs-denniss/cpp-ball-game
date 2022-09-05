#pragma once
#include "rendererComponent.hpp"

class CircleComponent : public RendererComponent {
private:
	friend class GUI;
public:
	CircleComponent(glm::vec4 color = glm::vec4(1.0f));
	~CircleComponent();
};
