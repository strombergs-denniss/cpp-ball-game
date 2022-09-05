#pragma once
#include "rendererSystem.hpp"
#include "../components/transformComponent.hpp"
#include "../components/lineComponent.hpp"

class LineSystem : public RendererSystem {
private:
	Camera* camera;
	Program* program;
	GLuint positionBuffer;
private:
	void prepareToRenderLines();
public:
	LineSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program);
	~LineSystem();
	void update() override;
};
