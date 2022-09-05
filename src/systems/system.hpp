#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <PlayRho/PlayRho.hpp>
#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/btBulletCollisionCommon.h>

class System {
protected:
	entt::DefaultRegistry* registry = nullptr;
public:
	System(entt::DefaultRegistry* registry);
	~System();
	virtual void update() = 0;
};
