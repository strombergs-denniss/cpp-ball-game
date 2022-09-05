#pragma once
#include "system.hpp"
#include "../components/transformComponent.hpp"
#include "../components/physics2DComponent.hpp"

class Physics2DSystem : public System {
public:
	pr2D::World world2D = pr2D::World();
	pr::StepConf stepConf2D = pr::StepConf();
	pr::Vec2 gravity2D = pr::Vec2(0, -10);
	Physics2DSystem(entt::DefaultRegistry* registry);
	void update() override;
	pr2D::World* getWorld();
};