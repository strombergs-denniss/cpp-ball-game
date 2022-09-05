#pragma once
#include "system.hpp"
#include "../components/transformComponent.hpp"
#include "../components/physics3DComponent.hpp"

class Physics3DSystem : public System {
public:
	friend class GUI;
	btVector3 gravity = btVector3(0.0f, -10.0f, 0.0f);
	btDefaultCollisionConfiguration* collisionConfiguration3D;
	btCollisionDispatcher* dispatcher3D;
	btBroadphaseInterface* pairCache3D;
	btSequentialImpulseConstraintSolver* constraintSolver3D;
	btDiscreteDynamicsWorld* world3D;
	float timeStep3D = 1.0f / 60.0f;
	Physics3DSystem(entt::DefaultRegistry* registry);
	void update() override;
	btDiscreteDynamicsWorld* getWorld();
	void applySettings();
};