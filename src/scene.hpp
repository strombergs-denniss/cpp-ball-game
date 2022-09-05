#pragma once
#include "jsonUtility.hpp"
#include "systems.hpp"
#include "input.hpp"
#include <iostream>
#include <fstream>

class Info {
public:
	uint32_t entity;
	std::string parentID;
public:
	Info();
	Info(uint32_t entity, std::string parentID);
	~Info();
};

class Scene {
public:
	std::map<std::string, GLuint>* textures;
private:
	friend class GUI;
	std::string name;
	entt::DefaultRegistry* registry = nullptr;
	Input* input = nullptr;
	Systems* systems = nullptr;
public:
	uint32_t root;
public:
	Scene(std::string name, Systems* systems);
	~Scene();
	void create();
	void setRegistry(entt::DefaultRegistry* registry);
	entt::DefaultRegistry* getRegistry();
	void setInput(Input* input);
	Input* getInput();
	void save();
	void load();
	void loadTexture(std::string name);
};
