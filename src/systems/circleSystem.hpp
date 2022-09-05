#pragma once
#include "rendererSystem.hpp"
#include "../components/transformComponent.hpp"
#include "../components/circleComponent.hpp"

void generateCircleVertices(float*& vertices, float*& uvs, int vertexCount);

class CircleSystem : public RendererSystem {
private:
	Camera* camera;
	Program* program;
	GLuint positionBuffer;
	GLuint texCoordBuffer;
	GLuint texture;
	int circleVertexCount = 24;
	float* circleVertices;
	float* uvs;
private:
	void prepareToRenderCircles();
public:
	CircleSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program);
	void update() override;
};