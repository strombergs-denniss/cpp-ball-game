#pragma once
#include "system.hpp"
#include "../components/transformComponent.hpp"
#include "../components/physics3DComponent.hpp"

class TransformSystem : public System {
private:
	btDiscreteDynamicsWorld* world3D;
private:
	TransformComponent& getTransformComponent(uint32_t entity);
public:
	TransformSystem(entt::DefaultRegistry* registry, btDiscreteDynamicsWorld* world3D);
	~TransformSystem();
	void setName(uint32_t entity, std::string name);
	const std::string& getName(uint32_t entity);
	uint32_t getParent(uint32_t entity);
	std::vector<uint32_t>* getChildren(uint32_t entity);
	void appendChild(uint32_t parent, uint32_t child);
	void clearChildren(uint32_t parent);
	void traverseTree(uint32_t entity);
	void destroyChild(uint32_t entity);
	void update() override;
};