#pragma once
#include "rendererSystem.hpp"
#include "../components/transformComponent.hpp"
#include "../components/pointComponent.hpp"

class PointSystem : public RendererSystem {
private:
	Program* program;
	GLuint positionBuffer;
	Camera* camera;
public:
	PointSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program);

	~PointSystem();
	void update() override;
};

