#pragma once
#include "systems/transformSystem.hpp"
#include "systems/pointSystem.hpp"
#include "systems/lineSystem.hpp"
#include "systems/shapeSystem.hpp"
#include "systems/rectangleSystem.hpp"
#include "systems/circleSystem.hpp"
#include "systems/cuboidSystem.hpp"
#include "systems/sphereSystem.hpp"
#include "systems/physics2DSystem.hpp"
#include "systems/physics3DSystem.hpp"

class Systems {
public:
	TransformSystem* transformSystem = nullptr;
	PointSystem* pointSystem = nullptr;
	LineSystem* lineSystem = nullptr;
	ShapeSystem* shapeSystem = nullptr;
	RectangleSystem* rectangleSystem = nullptr;
	CircleSystem* circleSystem = nullptr;
	CuboidSystem* cuboidSystem = nullptr;
	SphereSystem* sphereSystem = nullptr;
	Physics2DSystem* physics2DSystem = nullptr;
	Physics3DSystem* physics3DSystem = nullptr;
};
