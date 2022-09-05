#include "transformComponent.hpp"


/* Public */
TransformComponent::TransformComponent(std::string name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::string id) {
	this->name = name;
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;

	if (id == "") {
		std::random_device rd;
		std::mt19937_64 eng(rd());
		std::uniform_int_distribution<unsigned long long int> distr;
		unsigned long long int randomId = distr(eng);
		this->id = std::to_string(randomId);
	}
	else {
		this->id = id;
	}
}

TransformComponent::~TransformComponent() {}

void TransformComponent::setName(std::string name) {
	this->name = name;
}

std::string TransformComponent::getName() {
	return name;
}

uint32_t TransformComponent::getParent() {
	return parent;
}

std::vector<uint32_t>& TransformComponent::getChildren() {
	return children;
}

void TransformComponent::setPosition(glm::vec3 position) {
	this->position = position;
}

glm::vec3 TransformComponent::getPosition() {
	return position;
}

void TransformComponent::setRotation(glm::vec3 rotation) {
	this->rotation = rotation;
}

glm::vec3 TransformComponent::getRotation() {
	return rotation;
}

void TransformComponent::setScale(glm::vec3 scale) {
	this->scale = scale;
}

glm::vec3 TransformComponent::getScale() {
	return scale;
}

glm::mat4 TransformComponent::getModel() {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, scale);

	return model;
}

std::string TransformComponent::getID() {
	return id;
}

/* Public */
