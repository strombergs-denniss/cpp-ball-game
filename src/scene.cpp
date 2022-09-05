#include "scene.hpp"

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

Info::Info() {}

Info::Info(uint32_t entity, std::string parentID) {
	this->entity = entity;
	this->parentID = parentID;
}

Info::~Info() {}

Scene::Scene(std::string name, Systems* systems) {
	this->name = name;
	this->systems = systems;
}

Scene::~Scene() {}

void Scene::setRegistry(entt::DefaultRegistry* registry) {
	this->registry = registry;
}

entt::DefaultRegistry* Scene::getRegistry() {
	return registry;
}

void Scene::setInput(Input* input) {
	this->input = input;
}

Input* Scene::getInput() {
	return input;
}

void Scene::save() {
	entt::View view = registry->view<TransformComponent>();

	json sceneObject;
	json entityArray;

	for (uint32_t entity : view) {
		if (entity == 0) {
			continue;
		}

		json entityObject;
		json componentArray;

		if (registry->has<TransformComponent>(entity)) {
			TransformComponent& transform = registry->get<TransformComponent>(entity);

			json componentObject;
			componentObject["componentName"] = "TransformComponent";
			componentObject["index"] = entity;

			TransformComponent& parentTransform = registry->get<TransformComponent>(transform.getParent());
			
			componentObject["id"] = transform.getID();
			componentObject["parentID"] = parentTransform.getID();
			componentObject["name"] = transform.getName();
			componentObject["position"] = glmToJSON(transform.getPosition());
			componentObject["rotation"] = glmToJSON(transform.getRotation());
			componentObject["scale"] = glmToJSON(transform.getScale());
			componentArray.push_back(componentObject);
		}

		if (registry->has<PointComponent>(entity)) {
			PointComponent& point = registry->get<PointComponent>(entity);
			json componentObject;
			componentObject["componentName"] = "PointComponent";
			componentObject["vertices"] = glmToJSON(point.getVertices());
			componentObject["color"] = glmToJSON(point.getColor());

			componentArray.push_back(componentObject);
		}

		if (registry->has<LineComponent>(entity)) {
			const char** lineNames = LineComponent::getLineNames();

			LineComponent& line = registry->get<LineComponent>(entity);

			json componentObject;
			componentObject["componentName"] = "LineComponent";
			componentObject["vertices"] = glmToJSON(line.getVertices());
			componentObject["color"] = glmToJSON(line.getColor());
			componentObject["lineName"] = lineNames[line.getLineName()];
			componentArray.push_back(componentObject);
		}

		if (registry->has<ShapeComponent>(entity)) {
			const char** shapeNames = ShapeComponent::getShapeNames();

			ShapeComponent& shape = registry->get<ShapeComponent>(entity);

			json componentObject;
			componentObject["componentName"] = "LineComponent";
			componentObject["vertices"] = glmToJSON(shape.getVertices());
			componentObject["indices"] = glmToJSON(shape.getIndices());
			componentObject["colors"] = glmToJSON(shape.getColors());
			componentObject["shapeName"] = shapeNames[shape.getShapeName()];
			componentArray.push_back(componentObject);
		}

		if (registry->has<RectangleComponent>(entity)) {
			RectangleComponent& rectangle = registry->get<RectangleComponent>(entity);

			json componentObject;
			componentObject["componentName"] = "RectangleComponent";
			componentObject["color"] = glmToJSON(rectangle.getColor());
			componentObject["textureName"] = rectangle.getTextureName();
			componentArray.push_back(componentObject);
		}

		if (registry->has<CircleComponent>(entity)) {
			CircleComponent& circle = registry->get<CircleComponent>(entity);

			json componentObject;
			componentObject["componentName"] = "CircleComponent";
			componentObject["color"] = glmToJSON(circle.getColor());
			componentObject["textureName"] = circle.getTextureName();
			componentArray.push_back(componentObject);
		}

		if (registry->has<CuboidComponent>(entity)) {
			CuboidComponent& cuboid = registry->get<CuboidComponent>(entity);
			json componentObject;

			componentObject["componentName"] = "CuboidComponent";
			componentObject["color"] = glmToJSON(cuboid.getColor());
			componentObject["textureName"] = cuboid.getTextureName();
			componentArray.push_back(componentObject);
		}

		if (registry->has<SphereComponent>(entity)) {
			SphereComponent& sphere = registry->get<SphereComponent>(entity);

			json componentObject;
			componentObject["componentName"] = "SphereComponent";
			componentObject["color"] = glmToJSON(sphere.getColor());
			componentObject["textureName"] = sphere.getTextureName();
			componentArray.push_back(componentObject);
		}

		if (registry->has<Physics2DComponent>(entity)) {
			Physics2DComponent& physics2D = registry->get<Physics2DComponent>(entity);

			pr2D::Body* body = physics2D.getBody();

			json componentObject;
			componentObject["componentName"] = "Physics2DComponent";
			componentObject["location"] = json(body->GetLocation());
			componentObject["angle"] = glm::degrees(body->GetAngle());
			componentArray.push_back(componentObject);
		}

		if (registry->has<Physics3DComponent>(entity)) {
			Physics3DComponent& physics3D = registry->get<Physics3DComponent>(entity);
			
			btRigidBody* body = physics3D.getRigidBody();
			btTransform trans = body->getWorldTransform();
			btQuaternion quat = trans.getRotation();
			float x, y, z;
			quat.getEulerZYX(x, y, z);

			json componentObject;
			componentObject["componentName"] = "Physics3DComponent";
			componentObject["origin"] = btToJSON(trans.getOrigin());
			componentObject["rotation"] = {glm::degrees(z), glm::degrees(y), glm::degrees(x)};
			componentArray.push_back(componentObject);
		}

		entityObject["components"] = componentArray;
		entityArray.push_back(entityObject);
	}

	sceneObject["entities"] = entityArray;

	std::string sceneJSON = sceneObject.dump(4);
	std::ofstream sceneFile;
	sceneFile.open("levels\\" + name + "\\components.json");

	if (sceneFile.is_open()) {
		sceneFile << sceneJSON;
		sceneFile.close();
	}
	else {
		std::cout << "Unable to open or create file!\n";
	}
}

void Scene::create() {
	systems->transformSystem->clearChildren(root);
}

void Scene::loadTexture(std::string name) {
	if (name != "Default") {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		textures->insert(std::pair(name, texture));

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(("levels\\" + name + ".png").c_str(),
			&width,
			&height,
			&channels,
			STBI_rgb);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
}


void Scene::load() {
	std::ifstream sceneFile;
	sceneFile.open("levels\\" + name  + "\\components.json");

	std::ifstream physicsConfsFile;
	physicsConfsFile.open("levels\\" + name + "\\physicsConfs.json");

	entt::View view = registry->view<TransformComponent>();

	systems->transformSystem->clearChildren(root);

	
	if (sceneFile.is_open() && physicsConfsFile.is_open()) {
		std::map<std::string, Info> entities;
		entities["0"]= Info(0, "0");

		std::string sceneString((std::istreambuf_iterator<char>(sceneFile)), (std::istreambuf_iterator<char>()));
		std::string physicsConfsString((std::istreambuf_iterator<char>(physicsConfsFile)), (std::istreambuf_iterator<char>()));
		json sceneObject = json::parse(sceneString);
		json physicsConfsObject = json::parse(physicsConfsString);
		json entityArray = sceneObject["entities"];
		
		for (int a = entityArray.size(); a >= 0; a--) {
			json entityObject = entityArray[a];
			json componentArray = entityObject["components"];
			uint32_t entity = registry->create();
			std::string id;

			for (int b = 0; b < componentArray.size(); b++) {
				json componentObject = componentArray[b];

				if (componentObject["componentName"] == "TransformComponent") {
					std::string name = componentObject["name"];
					id = componentObject["id"];
					std::string parentID = componentObject["parentID"];
					glm::vec3 position = jsonToGLMVec3(componentObject["position"]);
					glm::vec3 rotation = jsonToGLMVec3(componentObject["rotation"]);
					glm::vec3 scale = jsonToGLMVec3(componentObject["scale"]);

					registry->assign<TransformComponent>(entity, name, position, rotation, scale, id);

					Info info(entity, parentID);
					entities.insert(std::pair(id, info));
				}
				
				else if (componentObject["componentName"] == "PointComponent") {
					std::vector<glm::vec3> vertices = jsonToGLMVec3Vector(componentObject["vertices"]);
					glm::vec4 color = jsonToGLMVec4(componentObject["color"]);
					registry->assign<PointComponent>(entity, vertices, color);
				}
				else if (componentObject["componentName"] == "LineComponent") {
					const char** lineNames = LineComponent::getLineNames();
					std::vector<glm::vec3> vertices = jsonToGLMVec3Vector(componentObject["vertices"]);
					glm::vec4 color = jsonToGLMVec4(componentObject["color"]);
					registry->assign<LineComponent>(entity, vertices, color);
				}
				else if (componentObject["componentName"] == "PolygonComponent") {
					std::string componentName = componentObject["componentName"];
					std::vector<glm::vec3> vertices = jsonToGLMVec3Vector(componentObject["vertices"]);
					std::vector<glm::uvec3> indices = jsonToGLMUvec3Vector(componentObject["indices"]);
					std::vector<glm::vec4> colors = jsonToGLMVec4Vector(componentObject["colors"]);
					registry->assign<ShapeComponent>(entity, vertices, indices, colors);
				}
				else if (componentObject["componentName"] == "RectangleComponent") {
					glm::vec4 color = jsonToGLMVec4(componentObject["color"]);
					registry->assign<RectangleComponent>(entity, color);

					RectangleComponent& rectangle = registry->get<RectangleComponent>(entity);

					rectangle.setTexture(componentObject["textureName"], (*textures)[componentObject["textureName"]]);
				}
				
				else if (componentObject["componentName"] == "CircleComponent") {
					glm::vec4 color = jsonToGLMVec4(componentObject["color"]);
					registry->assign<CircleComponent>(entity, color);

					CircleComponent& circle = registry->get<CircleComponent>(entity);
					circle.setTexture(componentObject["textureName"], (*textures)[componentObject["textureName"]]);
				}
				else if (componentObject["componentName"] == "CuboidComponent") {
					glm::vec4 color = jsonToGLMVec4(componentObject["color"]);
					registry->assign<CuboidComponent>(entity, color);

					CuboidComponent& cuboid = registry->get<CuboidComponent>(entity);
					cuboid.setTexture(componentObject["textureName"], (*textures)[componentObject["textureName"]]);
				}
				
				else if (componentObject["componentName"] == "SphereComponent") {
					glm::vec4 color = jsonToGLMVec4(componentObject["color"]);
					registry->assign<SphereComponent>(entity, color);

					SphereComponent& sphere = registry->get<SphereComponent>(entity);
					sphere.setTexture(componentObject["textureName"], (*textures)[componentObject["textureName"]]);
				}
				
				else if (componentObject["componentName"] == "Physics2DComponent") {
					if (physicsConfsObject.contains(id)) {
						json physicsConfObject = physicsConfsObject[id];

						Physics2DConf physics2DConf;
						physics2DConf.bodyConf.allowSleep = physicsConfObject["allowSleep"];
						physics2DConf.bodyConf.angle = componentObject["angle"];
						physics2DConf.bodyConf.angularAcceleration = physicsConfObject["angularAcceleration"];
						physics2DConf.bodyConf.angularDamping = physicsConfObject["angularDamping"];
						physics2DConf.bodyConf.angularVelocity = physicsConfObject["angularVelocity"];
						physics2DConf.bodyConf.awake = physicsConfObject["awake"];
						physics2DConf.bodyConf.enabled = physicsConfObject["enabled"];
						physics2DConf.bodyConf.fixedRotation = physicsConfObject["fixedRotation"];
						physics2DConf.bodyConf.linearAcceleration = jsonToPRVec2(physicsConfObject["linearAcceleration"]);
						physics2DConf.bodyConf.linearDamping = physicsConfObject["linearDamping"];
						physics2DConf.bodyConf.linearVelocity = jsonToPRVec2(physicsConfObject["linearVelocity"]);
						physics2DConf.bodyConf.location = jsonToPRVec2(componentObject["location"]);
						physics2DConf.bodyConf.type = static_cast<pr::BodyType>(physicsConfObject["type"]);
						physics2DConf.density = physicsConfObject["density"];
						physics2DConf.friction = physicsConfObject["friction"];
						physics2DConf.setAsBox = physicsConfObject["setAsBox"];
						physics2DConf.shape = static_cast<Shape2D>(physicsConfObject["shape"]);
						physics2DConf.scaling = jsonToPRVec2(physicsConfObject["scaling"]);
						physics2DConf.radius = physicsConfObject["radius"];
						physics2DConf.vertices = jsonToPRVec2Vector(physicsConfObject["vertices"]);

						registry->assign<Physics2DComponent>(entity, *systems->physics2DSystem->getWorld(), physics2DConf);
					}
				}
				else if (componentObject["componentName"] == "Physics3DComponent") {
					if (physicsConfsObject.contains(id)) {
						json physicsConfObject = physicsConfsObject[id];
						Physics3DConf physics3DConf;

						physics3DConf.mass = physicsConfObject["mass"];
						physics3DConf.shape = static_cast<Shape3D>(physicsConfObject["shape"]);
						physics3DConf.scaling = jsonToBTVector3(physicsConfObject["scaling"]);
						physics3DConf.radius = physicsConfObject["radius"];
						physics3DConf.origin = jsonToBTVector3(componentObject["origin"]);
						physics3DConf.rotation = jsonToBTVector3(componentObject["rotation"]);
						physics3DConf.angularDamping = physicsConfObject["angularDamping"];
						physics3DConf.angularSleepingThreshold = physicsConfObject["angularSleepingThreshold"];
						physics3DConf.friction = physicsConfObject["friction"];
						physics3DConf.linearDamping = physicsConfObject["linearDamping"];
						physics3DConf.linearSleepingThreshold = physicsConfObject["linearSleepingThreshold"];
						physics3DConf.restitution = physicsConfObject["restitution"];
						physics3DConf.rollingFriction = physicsConfObject["rollingFriction"];
						physics3DConf.spinningFriction = physicsConfObject["spinningFriction"];
						physics3DConf.userIndex = physicsConfObject["userIndex"];

						registry->assign<Physics3DComponent>(entity, *systems->physics3DSystem->world3D, physics3DConf);
					}
				}
			}
		}
		
		entt::View view = registry->view<TransformComponent>();

		for (auto entity : entities) {
			if (entity.first == "0") {
				continue;
			}

			systems->transformSystem->appendChild(entities[entity.second.parentID].entity, entity.second.entity);
		}

		sceneFile.close();
		physicsConfsFile.close();
	}
	else {
		std::cout << "Unable to open file!\n";
	}
}
