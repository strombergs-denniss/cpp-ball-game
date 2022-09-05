#include "rectangleSystem.hpp"

RectangleSystem::RectangleSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program) : RendererSystem(registry) {
	this->camera = camera;
	this->program = program;

	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &texCoordBuffer);

	program->setAttrib("a_TexCoord", 3, GL_FLOAT, GL_FALSE, 0);
}

void RectangleSystem::prepareToRenderRectangles() {
	program->use();

	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

	program->setAttrib("a_Position", 3, GL_FLOAT, GL_FALSE, 0);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	program->setAttrib("a_TexCoord", 2, GL_FLOAT, GL_FALSE, 0);

	program->setUniformMat4("u_Projection", camera->getProjection());
	program->setUniformMat4("u_View", camera->getView());
	glDisable(GL_CULL_FACE);
}

void RectangleSystem::update() {
	auto view = registry->view<TransformComponent, RectangleComponent>();
	prepareToRenderRectangles();

	for (auto entity : view) {
		auto& transform = view.get<TransformComponent>(entity);
		auto& rectangle = view.get<RectangleComponent>(entity);

		glBindTexture(GL_TEXTURE_2D, rectangle.getTexture());
		program->setUniform1i("u_Texture", 0);

		program->setUniformMat4("u_Model", transform.getModel());
		program->setUniformVec4("u_Color", rectangle.getColor());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}
