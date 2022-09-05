#pragma once
#include "rendererSystem.hpp"
#include "../components/transformComponent.hpp"
#include "../components/cuboidComponent.hpp"

class CuboidSystem : public RendererSystem {
	
	Camera* camera;
	Program* program;
	GLuint positionBuffer;
	GLuint texCoordBuffer;
	GLuint texture;
	float cuboidVertices[42] = {
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f
	};
	float texCoords[28] = {
		0, 0,
		1, 0,
		0, 1,
		1, 1,
		1, 0,
		0, 1,
		0, 0,
		1, 1,
		1, 0,
		0, 1,
		0, 0,
		1, 0,
		0, 1,
		1, 1
	};
private:
	void prepareToRenderCuboids();
public:
	CuboidSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program);
	void update() override;
};
