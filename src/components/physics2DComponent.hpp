#pragma once
#include <PlayRho/PlayRho.hpp>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace pr = playrho;
namespace pr2D = pr::d2;

enum Shape2D { CHAIN_SHAPE, DISK_SHAPE, EDGE_SHAPE, MULTI_SHAPE, POLYGON_SHAPE };

class Physics2DConf {
public:
	pr2D::BodyConf bodyConf;
	Shape2D shape = Shape2D::CHAIN_SHAPE;
	const char* shapes[5] = { "CHAIN_SHAPE", "DISK_SHAPE", "EDGE_SHAPE", "MULTI_SHAPE", "POLYGON_SHAPE" };
	const char* types[3] = { "Static", "Kinematic", "Dynamic"};
	float friction = 0.0f;
	float density = 0.0f;
	float restitution = 0.0f;
	float radius = 0.0f;
	std::vector<pr::Vec2> vertices;
	bool setAsBox = false;
	pr::Vec2 scaling = pr::Vec2(0.0f, 0.0f);
public:
	Physics2DConf();
	~Physics2DConf();
};

class Physics2DComponent {
private:
	pr2D::Body* body = nullptr;
	pr2D::Shape shape;
	pr2D::Fixture* fixture = nullptr;
public:
	bool isInitialized = false;
	Physics2DComponent();
	Physics2DComponent(pr2D::World& world, Physics2DConf& physics2DConf);
	~Physics2DComponent();
	void applyConf(pr2D::World& world, Physics2DConf& physics2DConf);
	pr2D::Body* getBody();
	pr2D::Shape getShape();
	pr2D::Fixture* getFixture();
};
