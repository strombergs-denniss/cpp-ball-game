#pragma once
#include "rendererSystem.hpp"
#include "../components/transformComponent.hpp"
#include "../components/shapeComponent.hpp"

class ShapeSystem : public RendererSystem {
private:
	Program* program;
	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint elementArrayBuffer;
	Camera* camera;
private:
	void prepareToRenderShapes();
public:
	ShapeSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program);
	void update() override;
};
