#include "circleSystem.hpp"

void generateCircleVertices(float*& vertices, float*& uvs, int vertexCount) {
	vertices = new float[(vertexCount + 2) * 3];
	uvs = new float[(vertexCount + 2) * 2];

	vertices[0] = 0.0f;
	vertices[1] = 0.0f;
	vertices[2] = 0.0f;
	uvs[0] = 0.0f * 0.5f + 0.5f;
	uvs[1] = 0.0f * 0.5f + 0.5f;

	int a = 1;
	for (; a < vertexCount; a++) {
		int b = a - 1;
		vertices[a * 3] = glm::cos(glm::pi<float>() * 2 / (vertexCount - 2) * b);
		vertices[a * 3 + 1] = glm::sin(glm::pi<float>() * 2 / (vertexCount - 2) * b);
		vertices[a * 3 + 2] = 0.0f;
		uvs[a * 2] = vertices[a * 3] * 0.5f + 0.5f;
		uvs[a * 2 + 1] = vertices[a * 3 + 1] * 0.5f + 0.5f;
	}

	a += 1;
	vertices[a * 3] = vertices[3];
	vertices[a * 3 + 1] = vertices[4];
	vertices[a * 3 + 2] = vertices[5];
	uvs[a * 2] = vertices[3] * 0.5 + 0.5;
	uvs[a * 2 + 1] = vertices[4] * 0.5 + 0.5;
}

void CircleSystem::prepareToRenderCircles() {
	program->use();
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circleVertexCount * 3, circleVertices, GL_STATIC_DRAW);
	program->setAttrib("a_Position", 3, GL_FLOAT, GL_FALSE, 0);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circleVertexCount * 2, uvs, GL_STATIC_DRAW);
	program->setAttrib("a_TexCoord", 2, GL_FLOAT, GL_FALSE, 0);

	program->setUniformMat4("u_Projection", camera->getProjection());
	program->setUniformMat4("u_View", camera->getView());
	glDisable(GL_CULL_FACE);
}

CircleSystem::CircleSystem(entt::DefaultRegistry * registry, Camera * camera, Program * program) : RendererSystem(registry) {
	this->camera = camera;
	this->program = program;
	
	generateCircleVertices(circleVertices, uvs, circleVertexCount);

	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &texCoordBuffer);

	program->setAttrib("a_TexCoord", 3, GL_FLOAT, GL_FALSE, 0);
}

void CircleSystem::update() {
	prepareToRenderCircles();

	auto entityView = registry->view<TransformComponent, CircleComponent>();

	for (auto entity : entityView) {
		auto& transform = entityView.get<TransformComponent>(entity);
		auto& circle = entityView.get<CircleComponent>(entity);

		glBindTexture(GL_TEXTURE_2D, circle.getTexture());
		program->setUniform1i("u_Texture", 0);

		program->setUniformMat4("u_Model", transform.getModel());
		program->setUniformVec4("u_Color", circle.getColor());
		glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertexCount);
	}
}