#include "system.hpp"

System::System(entt::DefaultRegistry* registry) {
	this->registry = registry;
}

System::~System() {}
