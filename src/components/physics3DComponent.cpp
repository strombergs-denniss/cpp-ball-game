#include "physics3DComponent.hpp"

/* Public */
Physics3DConf::Physics3DConf() {}

Physics3DConf::~Physics3DConf() {}

const char** Physics3DConf::getShapes() {
	const char* shapes[] = {"BOX", "SPHERE"};

	return shapes;
}

Physics3DComponent::Physics3DComponent() {}

Physics3DComponent::Physics3DComponent(btDiscreteDynamicsWorld& dynamicsWorld, Physics3DConf& physics3DConf) {
	this->dynamicsWorld = &dynamicsWorld;
	applyConf(dynamicsWorld, physics3DConf);
}

Physics3DComponent::~Physics3DComponent() {}

void Physics3DComponent::applyConf(btDiscreteDynamicsWorld& dynamicsWorld, Physics3DConf& physics3DConf) {
	physics3DConf.mass = glm::abs(physics3DConf.mass);
	//btClamp(physics3DConf.scaling, btVector3(0.0625f, 0.0625f, 0.0625f), btVector3(4096.0f, 4096.0f, 4096.0f));

	/* Motion State */
	btQuaternion quaternion = btQuaternion::getIdentity();
	quaternion.setEulerZYX(glm::radians(physics3DConf.rotation.getZ()), glm::radians(physics3DConf.rotation.getY()), glm::radians(physics3DConf.rotation.getX()));

	btTransform transform = btTransform::getIdentity();
	transform.setOrigin(physics3DConf.origin);
	transform.setRotation(quaternion);

	motionState = new btDefaultMotionState(transform);
	/* Motion State */

	/* Collision Shape */
	if (physics3DConf.shape == Shape3D::BOX) {
		collisionShape = new btBoxShape(physics3DConf.scaling / 2);
	}
	else if (physics3DConf.shape == Shape3D::SPHERE) {
		collisionShape = new btSphereShape(physics3DConf.radius);
	}

	/* Collision Shape */

	/* Local Inertia */
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	bool isDynamic = (physics3DConf.mass != 0.0f);

	if (isDynamic) {
		collisionShape->calculateLocalInertia(physics3DConf.mass, localInertia);
	}
	/* Local Inertia */

	/* Rigid Body */
	btRigidBody::btRigidBodyConstructionInfo* rigidBodyConstructionInfo = new btRigidBody::btRigidBodyConstructionInfo(physics3DConf.mass, motionState, collisionShape, localInertia);
	
	rigidBodyConstructionInfo->m_angularDamping = physics3DConf.angularDamping;
	rigidBodyConstructionInfo->m_angularSleepingThreshold = physics3DConf.angularSleepingThreshold;
	rigidBodyConstructionInfo->m_friction = physics3DConf.friction;
	rigidBodyConstructionInfo->m_linearDamping = physics3DConf.linearDamping;
	rigidBodyConstructionInfo->m_linearSleepingThreshold = physics3DConf.linearSleepingThreshold;
	rigidBodyConstructionInfo->m_restitution = physics3DConf.restitution;
	rigidBodyConstructionInfo->m_rollingFriction = physics3DConf.rollingFriction;
	rigidBodyConstructionInfo->m_spinningFriction = physics3DConf.spinningFriction;
	rigidBodyConstructionInfo->m_startWorldTransform;

	rigidBody = new btRigidBody(*rigidBodyConstructionInfo);
	rigidBody->setUserIndex(physics3DConf.userIndex);
	dynamicsWorld.addRigidBody(rigidBody);
	isInitialized = true;
	/* Rigid Body */
}

btRigidBody* Physics3DComponent::getRigidBody() {
	return rigidBody;
}

btDefaultMotionState* Physics3DComponent::getMotionState() {
	return motionState;
}

btCollisionShape* Physics3DComponent::getCollisionShape() {
	return collisionShape;
}
/* Public */
