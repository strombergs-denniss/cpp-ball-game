#include "lineSystem.hpp"

void LineSystem::prepareToRenderLines() {
	program->use();
	program->setUniformMat4("u_Projection", camera->getProjection());
	program->setUniformMat4("u_View", camera->getView());
}

LineSystem::LineSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program) : RendererSystem(registry) {
	this->camera = camera;
	this->program = program;

	glGenBuffers(1, &positionBuffer);
}

LineSystem::~LineSystem() {}

void LineSystem::update() {
	entt::View view = registry->view<TransformComponent, LineComponent>();

	for (uint32_t entity : view) {
		prepareToRenderLines();
		TransformComponent& transformComponent = registry->get<TransformComponent>(entity);
		LineComponent& lineComponent = registry->get<LineComponent>(entity);
		std::vector<glm::vec3>& points = lineComponent.getVertices();
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), &points.front(), GL_DYNAMIC_DRAW);
		program->setAttrib("a_Position", 3, GL_FLOAT, GL_FALSE, 0);
		program->setUniformMat4("u_Model", transformComponent.getModel());
		program->setUniformVec4("u_Color", lineComponent.getColors()[0]);
		glDrawArrays(lineComponent.getMode(), 0, points.size());
	}
}
