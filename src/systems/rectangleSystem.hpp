#pragma once
#include "rendererSystem.hpp"
#include "../components/transformComponent.hpp"
#include "../components/rectangleComponent.hpp"

class RectangleSystem : public RendererSystem {
private:
	GLuint texture;
	GLuint texCoordBuffer;
	Program* program;
	GLuint positionBuffer;
	Camera* camera;
	GLfloat rectangleVertices[12] = {
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};
	float texCoords[8] = {
		0, 0,
		1, 0,
		0, 1,
		1, 1
	};
private:
	void prepareToRenderRectangles();
public:
	RectangleSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program);

	void update() override;
};