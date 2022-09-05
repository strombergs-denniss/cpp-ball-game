#include "camera.hpp"

bool Camera::isTargetAcceptable(uint32_t* target) {
	if (cameraDimension == CameraDimension::CAMERA_2D) {
		if (registry->has<Physics2DComponent>(*target)) {
			return true;
		}
	}
	else {
		if (registry->has<Physics3DComponent>(*target)) {
			return true;
		}
	}

	return false;
}

Camera::Camera() {}

void Camera::detach() {
	if (cameraDimension == CameraDimension::CAMERA_2D) {
		processControls = &Camera::processControls2DNoTarget;
	}
	else {
		processControls = &Camera::processControls3DNoTarget;
	}

	hasTarget = false;
}

void Camera::jump() {
	if (hasTarget) {
		if (cameraDimension == CameraDimension::CAMERA_2D) {
			pr2D::ApplyLinearImpulse(*body2D, pr::Vec2(0, jumpForce2D), body2D->GetWorldCenter());
		}
		else {
			if (!jumpLocked) {
				body3D->applyCentralImpulse(btVector3(0.0f, jumpForce3D, 0.0f));
				jumpLocked = true;
			}
		}
	}
}

Camera::Camera(entt::DefaultRegistry* registry, Input* input, glm::vec3 eye, float zoom, uint32_t* target) {
	this->eye = eye;
	this->zoom = zoom;
	this->cameraDimension = CameraDimension::CAMERA_2D;
	this->input = input;

	if (target) {
		this->target = target;

		if (isTargetAcceptable(target)) {
			processControls = &Camera::processControls2DTarget;
			Physics2DComponent & physics2D = registry->get<Physics2DComponent>(*target);
			body2D = physics2D.getBody();
		}
		else {
			processControls = &Camera::processControls2DNoTarget;
		}
	}
	else {
		processControls = &Camera::processControls2DNoTarget;
	}
}

Camera::Camera(entt::DefaultRegistry* registry, Input* input, glm::vec3 eye, glm::vec3 center, float radius, uint32_t* target) {
	this->eye = eye;
	this->center = center;
	this->radius = radius;
	this->cameraDimension = CameraDimension::CAMERA_3D;
	this->input = input;

	if (target) {
		this->target = target;

		if (isTargetAcceptable(target)) {
			processControls = &Camera::processControls3DTarget;
			Physics3DComponent& physics3D = registry->get<Physics3DComponent>(*target);
			body3D = physics3D.getRigidBody();
		}
		else {
			processControls = &Camera::processControls3DNoTarget;
		}
	}
	else {
		processControls = &Camera::processControls3DNoTarget;
	}
}

Camera::~Camera() {}

void Camera::setTarget(Physics2DComponent& physics2D) {
	cameraDimension = CameraDimension::CAMERA_2D;
	processControls = &Camera::processControls2DTarget;
	this->body2D = physics2D.getBody();
	hasTarget = true;
}

void Camera::setTarget(Physics3DComponent& physics3D) {
	cameraDimension = CameraDimension::CAMERA_3D;
	processControls = &Camera::processControls3DTarget;
	this->body3D = physics3D.getRigidBody();
	hasTarget = true;
}

void Camera::setInput(Input* input) {
	this->input = input;
}

Input* Camera::getInput() {
	return input;
}

uint32_t* Camera::getTarget() {
	return target;
}

void Camera::setEye(glm::vec3 eye) {
	this->eye = eye;
}

glm::vec3 Camera::getEye() {
	return eye;
}

void Camera::setCenter(glm::vec3 center) {
	this->center = center;
}

glm::vec3 Camera::getCenter() {
	return center;
}

void Camera::setSpeed(float speed) {
	this->speed3D = speed;
}

float Camera::getSpeed() {
	return speed3D;
}

void Camera::setZoom(float zoom) {
	this->zoom = zoom;
}

float Camera::getZoom() {
	return zoom;
}

void Camera::setRadius(float radius) {
	radius = glm::clamp(radius, 0.1f, 100.0f);

	this->radius = radius;
}

float Camera::getRadius() {
	return radius;
}

void Camera::setSensitivity(float sensitivity) {
	this->sensitivity = sensitivity;
}

float Camera::getSensitivity() {
	return sensitivity;
}

void Camera::processControls2DTarget(glm::vec3 direction) {
	pr2D::ApplyLinearImpulse(*body2D, pr::Vec2(direction.x * movementSpeed2D, 0.0f), body2D->GetWorldCenter());
	pr::Vec2 location = body2D->GetLocation();
	eye = glm::vec3(location[0], location[1], 0.0f);
	projection = glm::ortho(-width / zoom, width / zoom, -height / zoom, height / zoom, -1.0f, 1.0f);
	view = glm::mat4(1.0f);
	view = glm::translate(view, -eye);
}

void Camera::processControls2DNoTarget(glm::vec3 direction) {
	projection = glm::ortho(-width / zoom, width / zoom, -height / zoom, height / zoom, -1.0f, 1.0f);
	eye += glm::vec3(direction.x * speed2D, direction.y * speed2D, 0.0f);
	view = glm::mat4(1.0f);
	view = glm::translate(view, -eye);
}


void Camera::processControls3DTarget(glm::vec3 direction) {
	updateYawAndPitch();

	btTransform transform = body3D->getWorldTransform();
	btVector3 origin = transform.getOrigin();
	btQuaternion quaternion = transform.getRotation();
	btVector3 rotation;
	float x, y, z;
	quaternion.getEulerZYX(x, y, z);
	rotation = btVector3(x, y, z);
	center = glm::vec3(origin.getX(), origin.getY(), origin.getZ());
	
	
	eye = center + glm::vec3(
		glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)) * radius,
		glm::sin(glm::radians(pitch)) * radius,
		glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch)) * radius
	);
	
	/*
	eye = center + glm::vec3(
		glm::sin(glm::radians(yaw))* glm::cos(glm::radians(pitch))* radius,
		glm::sin(glm::radians(pitch)) * radius,
		glm::cos(glm::radians(yaw))* glm::cos(glm::radians(pitch))* radius
	);
	*/

	glm::vec3 delta = eye - center;

	float length = glm::sqrt(delta.x * delta.x + delta.z * delta.z);

	glm::vec3 normalized = glm::vec3(delta.x / length, 0, delta.z / length);
	glm::vec3 right = glm::normalize(glm::cross(normalized, glm::vec3(0, 1, 0)));
	body3D->activate(true);
	body3D->applyCentralForce(btVector3(normalized.x * direction.z * movementSpeed3D, 0, normalized.z * direction.z * movementSpeed3D));
	body3D->applyCentralForce(btVector3(-right.x * direction.x * movementSpeed3D, 0, -right.z * direction.x * movementSpeed3D));

	projection = glm::perspective(glm::radians(fovy), (float)width / (float)height, zNear, zFar);
	view = glm::lookAt(eye, center, up);

	btVector3 btTo = btVector3(origin.getX(), -10000.0, origin.getZ());
	btCollisionWorld::ClosestRayResultCallback res(origin, btTo);

	world3D->rayTest(origin, btTo, res);

	if (res.hasHit()) {
		btVector3 delta = origin - res.m_hitPointWorld;
		btTransform trans;
		btVector3 min;
		btVector3 max;

		body3D->getCollisionShape()->getAabb(trans, min, max);
		btVector3 delta2 = max - min;

		if (delta.length() <=  delta2.getY() / 2.0f) {
			if (isTimerGoing && (res.m_collisionObject->getUserIndex() == 1 || res.m_collisionObject->getUserIndex() == 2)) {
				body3D->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
				body3D->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
				isTimerGoing = false;

				if (res.m_collisionObject->getUserIndex() == startIndex) {
					deltaTime = 0.0f;
				}

				startIndex = res.m_collisionObject->getUserIndex();
			}

			jumpLocked = false;
		}

		if (!isTimerGoing && !res.m_collisionObject->getUserIndex()) {
			startTime = glfwGetTime();
			isTimerGoing = true;
		}
	}

	if (isTimerGoing) {
		time = glfwGetTime();
		deltaTime = time - startTime;
	}

	if (origin.length() > limit) {
		restart();
	}
}

void Camera::restart() {
	if (body3D) {
		btTransform transform = body3D->getWorldTransform();
		body3D->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		body3D->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
		transform.setOrigin(startPoint);
		body3D->setWorldTransform(transform);
	}
}

void Camera::processControls3DNoTarget(glm::vec3 direction) {
	updateYawAndPitch();

	eye += Front * -direction.z * speed3D;
	eye += Right * direction.x * speed3D;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = -sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	Front = glm::normalize(front);
	Right = glm::normalize(glm::cross(Front, glm::vec3(0.0f, 1.0f, 0.0f)));
	Up = glm::normalize(glm::cross(Right, Front));

	projection = glm::perspective(glm::radians(fovy) , (float)width / (float)height, zNear, zFar);
	view = glm::lookAt(eye, eye + front, Up);

}

void Camera::update(glm::vec3 direction) {
	(this->*processControls)(direction);
}

glm::mat4 Camera::getProjection() {
	return projection;
}

glm::mat4 Camera::getView() {
	return view;
}

void Camera::setViewport(int width, int height) {
	this->width = width;
	this->height = height;
}

void Camera::setWorld3D(btDiscreteDynamicsWorld* world3D) {
	this->world3D = world3D;
}


void Camera::updateYawAndPitch() {
	double deltaMouseX = input->getDeltaX() * sensitivity;
	double deltaMouseY = input->getDeltaY() * sensitivity;
	yaw += deltaMouseX;
	pitch += deltaMouseY;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}

	if (pitch < -89.0f) {
		pitch = -89.0f;
	}
}