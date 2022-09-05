#include "physics2DComponent.hpp"

/* Public */
Physics2DConf::Physics2DConf() {}

Physics2DConf::~Physics2DConf() {}

Physics2DComponent::Physics2DComponent() {}

Physics2DComponent::Physics2DComponent(pr2D::World& world, Physics2DConf& physics2DConf) {
	applyConf(world, physics2DConf);
}

Physics2DComponent::~Physics2DComponent() {}

void Physics2DComponent::applyConf(pr2D::World& world, Physics2DConf& physics2DConf) {
	physics2DConf.bodyConf.angle = glm::radians(physics2DConf.bodyConf.angle);
	physics2DConf.density = glm::abs(physics2DConf.density);
	physics2DConf.radius = glm::clamp(physics2DConf.radius, 0.0625f, 4096.0f);
	physics2DConf.scaling[0] = glm::clamp(physics2DConf.scaling[0], 0.0625f, 4096.0f);
	physics2DConf.scaling[1] = glm::clamp(physics2DConf.scaling[1], 0.0625f, 4096.0f);
	body = world.CreateBody(physics2DConf.bodyConf);

	physics2DConf.density = glm::abs(physics2DConf.density);

	if (physics2DConf.vertices.size() == 0) {
		physics2DConf.vertices.push_back(pr::Vec2(0.0f, 0.0f));
	}

	if (physics2DConf.shape == Shape2D::CHAIN_SHAPE) {
		pr2D::ChainShapeConf chainShapeConf;
		chainShapeConf.restitution = physics2DConf.restitution;
		chainShapeConf.friction = physics2DConf.friction;
		chainShapeConf.density = physics2DConf.density;
		chainShapeConf.vertexRadius = physics2DConf.radius;
		chainShapeConf.Set(physics2DConf.vertices);
		shape = pr2D::Shape(chainShapeConf);
	}
	else if (physics2DConf.shape == Shape2D::DISK_SHAPE) {
		pr2D::DiskShapeConf diskShapeConf;
		diskShapeConf.restitution = physics2DConf.restitution;
		diskShapeConf.friction = physics2DConf.friction;
		diskShapeConf.density = physics2DConf.density;
		diskShapeConf.vertexRadius = physics2DConf.radius;
		shape = pr2D::Shape(diskShapeConf);
	}
	else if (physics2DConf.shape == Shape2D::EDGE_SHAPE) {
		pr2D::EdgeShapeConf edgeShapeConf;
		edgeShapeConf.restitution = physics2DConf.restitution;
		edgeShapeConf.friction = physics2DConf.friction;
		edgeShapeConf.density = physics2DConf.density;
		edgeShapeConf.vertexRadius = physics2DConf.radius;
		edgeShapeConf.Set(physics2DConf.vertices[0], physics2DConf.vertices[1]);
		shape = pr2D::Shape(edgeShapeConf);
	}
	else if (physics2DConf.shape == Shape2D::MULTI_SHAPE) {

	}
	else if (physics2DConf.shape == Shape2D::POLYGON_SHAPE) {
		pr2D::PolygonShapeConf polygonShapeConf;
		polygonShapeConf.restitution = physics2DConf.restitution;
		polygonShapeConf.friction = physics2DConf.friction;
		polygonShapeConf.density = physics2DConf.density;
		//polygonShapeConf.vertexRadius = physics2DConf.radius;

		if (physics2DConf.setAsBox) {
			polygonShapeConf.SetAsBox(physics2DConf.scaling[0] / 2.0f, physics2DConf.scaling[1] / 2.0f);
		}
		else {
			polygonShapeConf.UseVertices(physics2DConf.vertices);
		}

		shape = pr2D::Shape(polygonShapeConf);
	}

	
	fixture = body->CreateFixture(shape);
	isInitialized = true;
}

pr2D::Body* Physics2DComponent::getBody() {
	return body;
}

pr2D::Shape Physics2DComponent::getShape() {
	return shape;
}

pr2D::Fixture* Physics2DComponent::getFixture() {
	return fixture;
}
/* Public */
