#include "system.hpp"
#include "../texture.hpp"
#include "../program.hpp"
#include "../camera.hpp"

#pragma once
class RendererSystem : public System
{
private:
public:
	RendererSystem(entt::DefaultRegistry* registry);
	~RendererSystem();
	virtual void update() = 0;
};

