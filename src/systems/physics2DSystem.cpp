#include "physics2DSystem.hpp"

Physics2DSystem::Physics2DSystem(entt::DefaultRegistry* registry) : System(registry) {}

void Physics2DSystem::update() {
	auto entityView = registry->view<TransformComponent, Physics2DComponent>();
	world2D.Step(stepConf2D);

	for (auto entity : entityView) {
		auto& transform = entityView.get<TransformComponent>(entity);
		auto& rigidBody2D = entityView.get< Physics2DComponent>(entity);
		pr2D::Body* body = rigidBody2D.getBody();

		pr::Vec2 location = body->GetLocation();
		transform.setPosition(glm::vec3(location[0], location[1], 0.0f));
		transform.setRotation(glm::vec3(0.0f, glm::degrees(body->GetAngle()) , 0.0f));
	}
}

pr2D::World* Physics2DSystem::getWorld() {
	return &world2D;
}
