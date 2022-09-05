#pragma once
#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/btBulletCollisionCommon.h>
#include <glm/glm.hpp>
#include <iostream>

enum Shape3D {
	BOX,
	SPHERE
};

class Physics3DConf {
public:
	float mass = 0.0f;
	Shape3D shape = Shape3D::BOX;
	btVector3 scaling = btVector3(0.0f, 0.0f, 0.0f);
	btVector3 origin = btVector3(0.0f, 0.0f, 0.0f);
	btVector3 rotation = btVector3(0.0f, 0.0f, 0.0f);
	float radius = 0.0f;
	float angularDamping = 0.0f;
	float angularSleepingThreshold = 1.0f;
	float friction = 0.5f;
	float linearDamping = 0.0f;
	float linearSleepingThreshold = 0.8f;
	float restitution = 0.0f;
	float rollingFriction = 0.0f;
	float spinningFriction = 0.0f;
	int userIndex = 0;
	btTransform startWorldTransform = btTransform::getIdentity();
public:
	Physics3DConf();
	~Physics3DConf();
	static const char** getShapes();
};

class Physics3DComponent {
public:
	bool isInitialized = false;
	btRigidBody* rigidBody;
	btDefaultMotionState* motionState;
	btCollisionShape* collisionShape;
	btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
	Physics3DComponent();
	Physics3DComponent(btDiscreteDynamicsWorld& dynamicsWorld, Physics3DConf& physics3DConf);
	~Physics3DComponent();
	void applyConf(btDiscreteDynamicsWorld& dynamicsWorld, Physics3DConf& physics3DConf);
	btRigidBody* getRigidBody();
	btDefaultMotionState* getMotionState();
	btCollisionShape* getCollisionShape();
};
