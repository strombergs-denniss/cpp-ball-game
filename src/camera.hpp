#pragma once
#include <iostream>
#include "input.hpp"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "components/physics2DComponent.hpp"
#include "components/physics3DComponent.hpp"

enum CameraDimension { CAMERA_2D, CAMERA_3D };

class Camera {
private:
	friend class GUI;
	entt::DefaultRegistry* registry = nullptr;
	Input* input = nullptr;
	uint32_t* target = nullptr;
	int width, height;
	CameraDimension cameraDimension = CameraDimension::CAMERA_2D;
	glm::vec3 eye = glm::vec3(0.0f);
	glm::vec3 center = glm::vec3(0.0f);
	float speed2D = 1.0f;
	float speed3D = 1.0f;
	float movementSpeed2D = 100.0f;
	float movementSpeed3D = 100.0f;
	float zoom = 50.0f;
	float radius = 0.0f;
	float sensitivity = 0.1f;
	float yaw = 0.0f;
	float pitch = 0.0f;
	
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	void(Camera::* processControls)(glm::vec3 direction);
	pr2D::Body* body2D;
	btRigidBody* body3D;
	glm::vec3 Front = glm::vec3(0.0f);
	glm::vec3 Right = glm::vec3(0.0f);
	glm::vec3 Up = glm::vec3(0.0f);
	float fovy = 45;
	float zNear = 0.1f;
	float zFar = 1000.0f;
	float jumpForce2D = 10.0f;
	float jumpForce3D = 100.0f;
	bool hasTarget = false;
	float limit = 1000.0f;
	btDiscreteDynamicsWorld* world3D;
	bool jumpLocked = false;
	btVector3 startPoint = btVector3(0.0f, 20.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	bool isTimerGoing = false;
	float startTime;
	float time;
	float deltaTime;
	int startIndex;
private:
	bool isTargetAcceptable(uint32_t* target);
public:
	Camera();
	Camera(entt::DefaultRegistry* registry, Input* input, glm::vec3 eye, float zoom, uint32_t* target = nullptr);
	Camera(entt::DefaultRegistry* registry, Input* input, glm::vec3 eye, glm::vec3 center, float radius, uint32_t* target = nullptr);
	~Camera();
	void setInput(Input* input);
	Input* getInput();
	void setTarget(Physics2DComponent& physics2D);
	void setTarget(Physics3DComponent& physics3D);
	uint32_t* getTarget();
	void setEye(glm::vec3 eye);
	glm::vec3 getEye();
	void setCenter(glm::vec3 center);
	glm::vec3 getCenter();
	void setSpeed(float speed);
	float getSpeed();
	void setZoom(float zoom);
	float getZoom();
	void setRadius(float radius);
	float getRadius();
	void setSensitivity(float sensitivity);
	float getSensitivity();
	glm::mat4 getProjection();
	glm::mat4 getView();
	void processControls2DNoTarget(glm::vec3 direction);
	void processControls2DTarget(glm::vec3 direction);
	void processControls3DNoTarget(glm::vec3 direction);
	void processControls3DTarget(glm::vec3 direction);
	void update(glm::vec3 direction);
	void setViewport(int width, int height);
	void updateYawAndPitch();
	void detach();
	void jump();
	void setWorld3D(btDiscreteDynamicsWorld* world3D);
	void restart();
};
