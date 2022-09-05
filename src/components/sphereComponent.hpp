#pragma once
#include "circleComponent.hpp"

class SphereComponent : public CircleComponent {
public:
	SphereComponent(glm::vec4 color = glm::vec4(1.0f));
	~SphereComponent();
};
