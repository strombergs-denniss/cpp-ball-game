#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <random>
#include <iostream>

class TransformComponent {
public:
	glm::mat4 model;
private:
	friend class TransformSystem;
	friend class GUI;
	std::string id = "";
	std::string name;
	uint32_t parent;
	std::vector<uint32_t> children;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	bool isDestroyed = false;
public:
	TransformComponent(std::string name = "", glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), std::string id = "");
	~TransformComponent();
	void setName(std::string name);
	std::string getName();
	uint32_t getParent();
	std::vector<uint32_t>& getChildren();
	void setPosition(glm::vec3 position);
	glm::vec3 getPosition();
	void setRotation(glm::vec3 rotation);
	glm::vec3 getRotation();
	void setScale(glm::vec3 scales);
	glm::vec3 getScale();
	glm::mat4 getModel();
	std::string getID();
};
