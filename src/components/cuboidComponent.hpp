#pragma once
#include "rectangleComponent.hpp"

class CuboidComponent : public RectangleComponent {
public:
	CuboidComponent(glm::vec4 color = glm::vec4(1.0f));
	~CuboidComponent();
};
