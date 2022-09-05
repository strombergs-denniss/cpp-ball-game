#include "pointSystem.hpp"

PointSystem::PointSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program) : RendererSystem(registry) {
	this->program = program;
	this->camera = camera;

	glGenBuffers(1, &positionBuffer);
}

PointSystem::~PointSystem() {}

void PointSystem::update() {
	entt::View view = registry->view<TransformComponent, PointComponent>();

	for (uint32_t entity : view) {
		TransformComponent& transformComponent = registry->get<TransformComponent>(entity);
		PointComponent& pointComponent = registry->get<PointComponent>(entity);
		std::vector<glm::vec3>& points = pointComponent.getVertices();

		program->use();
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), &points.front(), GL_DYNAMIC_DRAW);
		program->setAttrib("a_Position", 3, GL_FLOAT, GL_FALSE, 0);
		program->setUniformMat4("u_Projection", camera->getProjection());
		program->setUniformMat4("u_View", camera->getView());
		program->setUniformMat4("u_Model", transformComponent.getModel());
		program->setUniformVec4("u_Color", pointComponent.getColors()[0]);
		glDrawArrays(GL_POINTS, 0, points.size());
	}
}
