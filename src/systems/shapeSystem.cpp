#include "shapeSystem.hpp"

/* Private */
void ShapeSystem::prepareToRenderShapes() {
	program->use();
	program->setUniformMat4("u_Projection", camera->getProjection());
	program->setUniformMat4("u_View", camera->getView());
}
/* Private */

/* Public */
ShapeSystem::ShapeSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program) : RendererSystem(registry){
	this->camera = camera;
	this->program = program;
	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &colorBuffer);
	glGenBuffers(1, &elementArrayBuffer);
}

void ShapeSystem::update() {
	prepareToRenderShapes();

	auto entityView = registry->view<TransformComponent, ShapeComponent>();

	for (auto entity : entityView) {
		auto& transform = entityView.get<TransformComponent>(entity);
		auto& shape = entityView.get<ShapeComponent>(entity);
		std::vector<glm::vec3>& vertices = shape.getVertices();
		std::vector<glm::vec4>& colors = shape.getColors();
		std::vector<glm::uvec3>& indices = shape.getIndices();

		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_DYNAMIC_DRAW);
		program->setAttrib("a_Color", 4, GL_FLOAT, GL_FALSE, 0);

		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
		program->setAttrib("a_Position", 3, GL_FLOAT, GL_FALSE, 0);

		if (shape.isIndexed()) {
			program->setUniformMat4("u_Model", transform.getModel());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec3), indices.data(), GL_DYNAMIC_DRAW);
			glDrawElements(shape.getMode(), indices.size() * 3, GL_UNSIGNED_INT, nullptr);
		}
		else {
			glDrawArrays(shape.getMode(), 0, vertices.size());
		}

		//program->setUniformVec4("u_Color", colors[0]);
		
	}
}
/* Public */
