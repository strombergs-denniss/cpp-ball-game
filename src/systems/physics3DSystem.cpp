#include "physics3DSystem.hpp"

Physics3DSystem::Physics3DSystem(entt::DefaultRegistry* registry) : System(registry) {
	collisionConfiguration3D = new btDefaultCollisionConfiguration();
	dispatcher3D = new btCollisionDispatcher(collisionConfiguration3D);
	pairCache3D = new btDbvtBroadphase();
	constraintSolver3D = new btSequentialImpulseConstraintSolver;
	world3D = new btDiscreteDynamicsWorld(dispatcher3D, pairCache3D, constraintSolver3D, collisionConfiguration3D);
	world3D->setGravity(gravity);
}

void Physics3DSystem::applySettings() {
	world3D->setGravity(gravity);
}

void Physics3DSystem::update() {
	world3D->stepSimulation(timeStep3D);

	auto view = registry->view<TransformComponent, Physics3DComponent>();

	for (uint32_t entity : view) {
		TransformComponent& transformComp = view.get<TransformComponent>(entity);
		Physics3DComponent& physics3DComp = view.get<Physics3DComponent>(entity);
		btRigidBody* rigidBody = physics3DComp.getRigidBody();

		btTransform transform;
		rigidBody->getMotionState()->getWorldTransform(transform);

		btVector3 origin = transform.getOrigin();
		btQuaternion btRotation = transform.getRotation();
		
		glm::quat glmRotation = glm::identity<glm::quat>();
		glmRotation.x = btRotation.getX();
		glmRotation.y = btRotation.getZ();
		glmRotation.z = btRotation.getY();
		glmRotation.w = btRotation.getW();

		glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(glmRotation));

		transformComp.setPosition(glm::vec3(origin.getX(), origin.getY(), origin.getZ()));
		transformComp.setRotation(eulerAngles);
	}
}

btDiscreteDynamicsWorld* Physics3DSystem::getWorld() {
	return world3D;
}