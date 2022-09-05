#include "cuboidSystem.hpp"

void CuboidSystem::prepareToRenderCuboids() {
	program->use();
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cuboidVertices), cuboidVertices, GL_STATIC_DRAW);
	program->setAttrib("a_Position", 3, GL_FLOAT, GL_FALSE, 0);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	program->setAttrib("a_TexCoord", 2, GL_FLOAT, GL_FALSE, 0);
	
	program->setUniformMat4("u_Projection", camera->getProjection());
	program->setUniformMat4("u_View", camera->getView());

	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
}

CuboidSystem::CuboidSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program) : RendererSystem(registry) {
	this->program = program;
	this->camera = camera;

	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &texCoordBuffer);

	program->setAttrib("a_TexCoord", 3, GL_FLOAT, GL_FALSE, 0);
}

void CuboidSystem::update() {
	prepareToRenderCuboids();

	auto entityView = registry->view<TransformComponent, CuboidComponent>();

	for (auto entity : entityView) {
		auto& transform = entityView.get<TransformComponent>(entity);
		auto& cuboid = entityView.get<CuboidComponent>(entity);

		glBindTexture(GL_TEXTURE_2D, cuboid.getTexture());
		program->setUniform1i("u_Texture", 0);
		program->setUniformMat4("u_Model", transform.getModel());
		program->setUniformVec4("u_Color", cuboid.getColor());

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
	}
}
