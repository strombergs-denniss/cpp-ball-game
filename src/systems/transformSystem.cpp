#include "transformSystem.hpp"

TransformComponent& TransformSystem::getTransformComponent(uint32_t entity) {
	return registry->get<TransformComponent>(entity);
}

TransformSystem::TransformSystem(entt::DefaultRegistry* registry, btDiscreteDynamicsWorld* world3D) : System(registry) {
	this->world3D = world3D;
};

TransformSystem::~TransformSystem() {}

void TransformSystem::setName(uint32_t entity, std::string name) {
	getTransformComponent(entity).name = name;
}

const std::string& TransformSystem::getName(uint32_t entity) {
	return getTransformComponent(entity).name;
}

uint32_t TransformSystem::getParent(uint32_t entity) {
	return getTransformComponent(entity).parent;
}

std::vector<uint32_t>* TransformSystem::getChildren(uint32_t entity) {
	return &getTransformComponent(entity).children;
}

void TransformSystem::appendChild(uint32_t parent, uint32_t child) {
	TransformComponent& parentTransform = registry->get<TransformComponent>(parent);
	TransformComponent& childTransform = registry->get<TransformComponent>(child);
	parentTransform.children.push_back(child);
	childTransform.parent = parent;
}

void TransformSystem::clearChildren(uint32_t parent) {
	TransformComponent& parentTransform = registry->get<TransformComponent>(parent);

	for (int a = 0; a < parentTransform.children.size(); a++) {
		if (registry->has<TransformComponent>(parentTransform.children[a])) {
			TransformComponent& childTransform = registry->get<TransformComponent>(parentTransform.children[a]);
			
			if (registry->has<Physics3DComponent>(parentTransform.children[a])) {
				Physics3DComponent& childPhysics3D = registry->get<Physics3DComponent>(parentTransform.children[a]);
				world3D->removeCollisionObject(childPhysics3D.getRigidBody());
			}
			
			clearChildren(parentTransform.children[a]);
			registry->destroy(parentTransform.children[a]);
		}
	}

	parentTransform.children.clear();
}

void TransformSystem::destroyChild(uint32_t child) {
	clearChildren(child);

	if (child) {
		TransformComponent& transform = getTransformComponent(child);
		TransformComponent& parentTransform = registry->get<TransformComponent>(transform.parent);
		std::cout << parentTransform.getName() << "\n";

		for (int a = 0; a < parentTransform.children.size(); a++) {
			if (parentTransform.children[a] == child) {
				parentTransform.children.erase(parentTransform.children.begin() + a);
				transform.isDestroyed = true;
				break;
			}
		}
	}
}

void TransformSystem::traverseTree(uint32_t entity) {
	if (registry->has<TransformComponent>(entity)) {
		TransformComponent& transformComponent = registry->get<TransformComponent>(entity);

		for (int a = 0; a < transformComponent.children.size(); a++) {
			traverseTree(transformComponent.children[a]);
		}
	}
}



void TransformSystem::update() {

}