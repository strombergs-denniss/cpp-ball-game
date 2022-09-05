// Dear ImGui for GUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_stdlib.h"

// gl3w and GLFW for OpenGL and window
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// GLM for mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

// EnTT and JSON for ECS (Entity Component System) and JSON
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

// Main
#include "program.hpp"
#include "scene.hpp"
#include "input.hpp"
#include "camera.hpp"
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

static glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

static Systems systems;

void processKeyboardInput(GLFWwindow* window, float& directionX, float& directionY, float& directionZ) {
	directionX = 0.0f;
	directionY = 0.0f;
	directionZ = 0.0f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		directionZ = -1;
		directionY = 1;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		directionZ = 1;
		directionY = -1;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		directionX = -1;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		directionX = 1;
	}
}

void InputBool(const char* label, bool* v) {
	ImGui::Text(label);

	if (ImGui::RadioButton("True", *v)) {
		*v = true;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("False", !*v)) {
		*v = false;
	}
}

std::string prVec2ToString(pr::Vec2 vec) {
	return "x: " + std::to_string(vec[0]) + " ; y: " + std::to_string(vec[1]);
}

std::string glmVec3ToString(glm::vec3 vec) {
	return "x: " + std::to_string(vec.x) + " ; y: " + std::to_string(vec.y) + " ; z: " + std::to_string(vec.z);
}

enum Dimension {D3 };
enum Pattern { CUSTOM, GRID, };

class RandomizerConf {
public:
	const char* dimensions[2] = { "2D", "3D" };
	Dimension dimension = Dimension::D3;
	glm::vec3 positionMin = glm::vec3(0.0f);
	glm::vec3 positionMax = glm::vec3(0.0f);
	glm::vec3 scaleMin = glm::vec3(0.0f);
	glm::vec3 scaleMax = glm::vec3(0.0f);
	glm::vec3 rotationMin = glm::vec3(0.0f);
	glm::vec3 rotationMax = glm::vec3(0.0f);
	ShapeName shape = ShapeName::RECTANGLE;
	glm::vec4 colorMin = glm::vec4(1.0f);
	glm::vec4 colorMax = glm::vec4(1.0f);
	Pattern pattern = Pattern::CUSTOM;
	const char* patterns[2] = { "CUSTOM", "GRID" };
	int objectCountMin = 0;
	int objectCountMax = 0;
	glm::uvec2 grid = glm::uvec2(0);
	bool usePhysics = false;
	std::string textureName = "Default";
	GLuint texture;
};

std::map<std::string, GLuint> textures;

class CopyBuffer {
public:
	TransformComponent transform;
	Physics2DComponent physics2D;
	Physics3DComponent physics3D;
	Physics2DConf physics2DConf;
	Physics3DConf physics3DConf;
	PointComponent point;
	LineComponent line;
	ShapeComponent shape;
	RectangleComponent rectangle;
	CircleComponent circle;
	CuboidComponent cuboid;
	SphereComponent sphere;
};

class GUI {
private:
	entt::DefaultRegistry* registry = nullptr;
	Camera* camera = nullptr;
	bool areSettingsVisble = false;
	bool areSettingsCollapsed = false;
	bool isRandomizerVisible = false;
	bool isRandomizerCollapsed = false;
	bool isEntityComponentViewVisible = false;
	bool isEntityComponentViewCollapsed = false;
	bool isEntityTreeViewVisible = false;
	bool isEntityTreeViewCollapsed = false;
	bool isSceneLoadingMenuVisible = false;
	bool isSceneCreationMenuVisible = false;
	bool isMainMenuBarVisible = false;
	bool isMainMenuVisible = true;
	int mainMenuState = 0;
	uint32_t selectedEntity = 0;
	Scene* scene = nullptr;
	GLFWwindow* window = nullptr;
	std::vector<std::string> levels;
	int selectedLevel = 0;
	CopyBuffer copyBuffer;
public:
	GUI(entt::DefaultRegistry* registry, Camera* camera, Scene* scene, GLFWwindow* window) {
		this->registry = registry;
		this->camera = camera;
		this->scene = scene;
	}

	~GUI() {}

	void setSelectedEntity(uint32_t selectedEntity) {
		this->selectedEntity = selectedEntity;
	}

	void traverseEntityTree(entt::DefaultRegistry& registry, TransformSystem& transformSystem, uint32_t parent) {
		TransformComponent& parentTransformComponent = registry.get<TransformComponent>(parent);
		std::vector<uint32_t>& children = parentTransformComponent.getChildren();

		TransformComponent& seelectedTransformComponent = registry.get<TransformComponent>(selectedEntity);

		ImGui::PushID(parent);
		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (selectedEntity && selectedEntity == parent) {
			treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		bool isTreeNodeOpen = ImGui::TreeNodeEx(parentTransformComponent.getName().c_str(), treeNodeFlags);

		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {
			selectedEntity = parent;
		}

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Add empty entity")) {
				auto entity = registry.create();
				registry.assign<TransformComponent>(entity, "Entity");

				transformSystem.appendChild(selectedEntity, entity);
				ImGui::CloseCurrentPopup();
			}
			
			/*
			if (ImGui::MenuItem("Copy")) {
				copyBuffer.transform = registry.get<TransformComponent>(selectedEntity);

				if (registry.has<Physics2DComponent>(selectedEntity)) {
					copyBuffer.physics2D = registry.get<Physics2DComponent>(selectedEntity);
				}

				if (registry.has<Physics3DComponent>(selectedEntity)) {
					copyBuffer.physics3D = registry.get<Physics3DComponent>(selectedEntity);
				}

				if (registry.has<Physics2DConf>(selectedEntity)) {
					copyBuffer.physics2DConf = registry.get<Physics2DConf>(selectedEntity);
				}

				if (registry.has<Physics3DConf>(selectedEntity)) {
					copyBuffer.physics3DConf = registry.get<Physics3DConf>(selectedEntity);
				}

				if (registry.has<PointComponent>(selectedEntity)) {
					copyBuffer.point = registry.get<PointComponent>(selectedEntity);
				}

				if (registry.has<LineComponent>(selectedEntity)) {
					copyBuffer.line = registry.get<LineComponent>(selectedEntity);
				}

				if (registry.has<ShapeComponent>(selectedEntity)) {
					copyBuffer.shape = registry.get<ShapeComponent>(selectedEntity);
				}

				if (registry.has<RectangleComponent>(selectedEntity)) {
					copyBuffer.rectangle = registry.get<RectangleComponent>(selectedEntity);
				}

				if (registry.has<CircleComponent>(selectedEntity)) {
					copyBuffer.circle = registry.get<CircleComponent>(selectedEntity);
				}

				if (registry.has<CuboidComponent>(selectedEntity)) {
					copyBuffer.cuboid = registry.get<CuboidComponent>(selectedEntity);
				}

				if (registry.has<SphereComponent>(selectedEntity)) {
					copyBuffer.sphere = registry.get<SphereComponent>(selectedEntity);
				}
			}
			*/

			ImGui::EndPopup();
		}

		if (isTreeNodeOpen) {
			for (int a = 0; a < children.size(); a++) {
				TransformComponent& comp = registry.get<TransformComponent>(children[a]);

				ImGui::PushID(&children[a]);
				traverseEntityTree(registry, transformSystem, children[a]);
				ImGui::PopID();
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void displaySceneCreation() {
		ImGui::SetNextWindowPosCenter();

		if (ImGui::BeginPopupModal("SceneCreation", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("Name", &scene->name);

			if (ImGui::Button("Create")) {
				if (scene->name != "") {
					system(("mkdir levels\\" + scene->name).c_str());
					scene->create();
					isSceneCreationMenuVisible = false;

					std::ofstream physicsConfsFile;
					physicsConfsFile.open("levels\\" + scene->name + "\\physicsConfs.json");

					json physicsConfs = json::object();

					std::string physicsConfsJson = physicsConfs.dump(4);

					if (physicsConfsFile.is_open()) {
						physicsConfsFile << physicsConfsJson;
					}
					else {
						std::cout << "Unable to open or create file!\n";
					}

					physicsConfsFile.close();

					std::ifstream rLevelsFile;
					rLevelsFile.open("levels\\levels.json");
					json levelsJSON;

					std::string levelsString((std::istreambuf_iterator<char>(rLevelsFile)), (std::istreambuf_iterator<char>()));

					if (levelsString.size() > 0) {
						levelsJSON = json::parse(levelsString);
					}

					json level;
					level["name"] = scene->name;

					levelsJSON[scene->name] = level;

					rLevelsFile.close();

					std::ofstream wLevelsFile;
					wLevelsFile.open("levels\\levels.json", std::ofstream::trunc);

					if (wLevelsFile.is_open()) {
						wLevelsFile << levelsJSON.dump(4);
					}

					wLevelsFile.close();

					loadLevels();

					ImGui::CloseCurrentPopup();

				}
			}

			if (ImGui::Button("Close")) {
				isSceneCreationMenuVisible = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void loadLevels() {
		std::ifstream levelsFile;
		levelsFile.open("levels\\levels.json");

		if (levelsFile.is_open()) {
			std::string levelName;
			levels.clear();

			std::string levelsString((std::istreambuf_iterator<char>(levelsFile)), (std::istreambuf_iterator<char>()));

			if (levelsString.size() > 0) {
				json levelsJSON = json::parse(levelsString);

				for (auto level : levelsJSON) {
					levels.push_back(level["name"]);
				}
			}
		}

		levelsFile.close();
	}

	void removeLevel(std::string levelName) {
		std::ifstream rLevelsFile;
		rLevelsFile.open("levels\\levels.json");

		json levelsJSON;

		if (rLevelsFile.is_open()) {
			std::string levelsString((std::istreambuf_iterator<char>(rLevelsFile)), (std::istreambuf_iterator<char>()));

			if (levelsString.size() > 0) {
				levelsJSON = json::parse(levelsString);
				levelsJSON.erase(levelName);
			}

			rLevelsFile.close();
		}

		std::ofstream wLevelsFile;
		wLevelsFile.open("levels\\levels.json", std::ofstream::trunc);

		if (wLevelsFile.is_open()) {
			wLevelsFile << levelsJSON.dump(4);
			wLevelsFile.close();
		}
	}

	void displayMainMenu() {
		if (isMainMenuVisible) {
			ImGui::SetNextWindowPosCenter();

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

			bool shouldLoadLevels = false;

			if (mainMenuState == 0) {
				ImGui::Begin("###MainMenu", nullptr, windowFlags);

				ImVec2 fullWidth = ImVec2(ImGui::GetContentRegionAvail().x, 0.0f);

				if (ImGui::Button("Play", fullWidth)) {
					mainMenuState = 1;
					shouldLoadLevels = true;
				}

				if (ImGui::Button("Level Editor", fullWidth)) {
					mainMenuState = 2;
					shouldLoadLevels = true;
				}

				if (ImGui::Button("Settings", fullWidth)) {
					mainMenuState = 3;
				}

				if (ImGui::Button("Exit", fullWidth)) {

				}

				if (shouldLoadLevels) {
					loadLevels();
				}

				ImGui::End();
			}

			if (mainMenuState == 1) {
				ImGui::Begin("Play###MainMenu", nullptr, windowFlags);

				ImVec2 fullWidth = ImVec2(ImGui::GetContentRegionAvail().x, 0.0f);

				if (ImGui::Button("Back")) {
					mainMenuState = 0;
				}

				ImGui::Spacing();

				for (int a = 0; a < levels.size(); a++) {
					if (ImGui::Button(levels[a].c_str(), fullWidth)) {
						scene->name = levels[a];
						scene->load();
						loadSettings();

						auto view = registry->view<TransformComponent>();

						for (auto entity : view) {
							TransformComponent& transform = view.get(entity);

							std::string searchName = "";

							if (camera->cameraDimension == CameraDimension::CAMERA_2D) {
								searchName = "Player2D";
							}
							else {
								searchName = "Player3D";
							}

							if (transform.name == searchName) {
								if (registry->has<Physics2DComponent>(entity)) {
									Physics2DComponent& physics2D = registry->get<Physics2DComponent>(entity);
									camera->setTarget(physics2D);
								}
								else if (registry->has<Physics3DComponent>(entity)) {
									Physics3DComponent& physics3D = registry->get<Physics3DComponent>(entity);
									camera->setTarget(physics3D);
								}
							}

							if (transform.name == "Start") {
								if (registry->has<Physics3DComponent>(entity)) {
									Physics3DComponent& physics3D = registry->get<Physics3DComponent>(entity);
									btTransform transform = physics3D.getRigidBody()->getWorldTransform();
									camera->startPoint = transform.getOrigin() + btVector3(0.0f, physics3D.collisionShape->getLocalScaling().getY() + 10.0f, 0.0f);
								}

							}
						}

						mainMenuState = 0;
						isMainMenuVisible = false;
					}
				}

				ImGui::End();
			}

			if (mainMenuState == 2) {
				ImGui::Begin("Level Editor###MainMenu", nullptr, windowFlags);

				if (ImGui::Button("Back")) {
					mainMenuState = 0;
				}

				ImGui::Spacing();

				if (ImGui::Button("New")) {
					ImGui::OpenPopup("SceneCreation");
				}

				displaySceneCreation();

				ImGui::SameLine();

				if (ImGui::Button("Edit")) {
					if (levels.size() > 0) {
						scene->name = levels[selectedLevel];
						scene->load();
						loadSettings();
						mainMenuState = 0;
						isMainMenuVisible = false;
						isMainMenuBarVisible = true;
						isEntityComponentViewVisible = true;
						isRandomizerVisible = true;
						isEntityTreeViewVisible = true;
						areSettingsVisble = true;
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Delete")) {
					if (levels.size() > 0) {
						std::filesystem::remove_all("levels\\" + levels[selectedLevel]);
						removeLevel(levels[selectedLevel]);

						loadLevels();
					}
				}

				ImGui::Spacing();

				ImGui::BeginChild("Level");

				for (int a = 0; a < levels.size(); a++) {
					if (ImGui::Selectable(levels[a].c_str(), a == selectedLevel)) {
						selectedLevel = a;
					}
				}

				ImGui::EndChild();
				ImGui::End();
			}

			if (mainMenuState == 3) {
				ImGui::Begin("Settings###MainMenu", nullptr, windowFlags);

				if (ImGui::Button("Back")) {
					mainMenuState = 0;
				}

				ImGui::End();
			}
		}
	}

	void close() {
		isMainMenuVisible = true;
		isMainMenuBarVisible = false;
		isRandomizerVisible = false;
		isEntityTreeViewVisible = false;
		isEntityComponentViewVisible = false;
		areSettingsVisble = false;
	}

	void displayMainMenuBar() {
		if (isMainMenuBarVisible) {
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					ImGui::PushID(0);
					/*
					if (ImGui::MenuItem("New")) {
						isSceneCreationMenuVisible = true;
					}
					*/

					if (ImGui::MenuItem("Save")) {
						scene->save();
						saveSettings();
					}

					if (ImGui::MenuItem("Close")) {
						close();
					}
					ImGui::PopID();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("View")) {
					ImGui::Checkbox("Tree View", &isEntityTreeViewVisible);
					ImGui::Checkbox("Settings", &areSettingsVisble);
					ImGui::Checkbox("Entity Component View", &isEntityComponentViewVisible);
					ImGui::Checkbox("Randomizer", &isRandomizerVisible);

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}

			if (isSceneCreationMenuVisible) {
				ImGui::OpenPopup("SceneCreation");
			}
			else if (isSceneLoadingMenuVisible) {
				ImGui::OpenPopup("SceneLoading");
			}

			displaySceneCreation();

			if (ImGui::BeginPopupModal("SceneLoading")) {
				ImGui::InputText("Name", &scene->name);

				if (ImGui::Button("Load")) {
					scene->load();
					loadSettings();
					isSceneLoadingMenuVisible = false;
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Close")) {
					isSceneLoadingMenuVisible = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
	}

	void saveSettings() {
		std::ofstream settingsFile;
		settingsFile.open("levels\\" + scene->name + "\\settings.json", std::fstream::trunc);

		if (settingsFile.is_open()) {
			json settingsJSON;

			settingsJSON["gravity"] = btToJSON(systems.physics3DSystem->gravity);
			settingsJSON["radius"] = camera->radius;
			settingsJSON["speed2D"] = camera->speed2D;
			settingsJSON["speed3D"] = camera->speed3D;
			settingsJSON["movementSpeed2D"] = camera->movementSpeed2D;
			settingsJSON["movementSpeed3D"] = camera->movementSpeed3D;
			settingsJSON["zoom"] = camera->zoom;
			settingsJSON["fovy"] = camera->fovy;
			settingsJSON["zNear"] = camera->zNear;
			settingsJSON["zFar"] = camera->zFar;
			settingsJSON["eye"] = glmToJSON(camera->eye);
			settingsJSON["jumpForce2D"] = camera->jumpForce2D;
			settingsJSON["jumpForce3D"] = camera->jumpForce3D;
			settingsJSON["clearColor"] = glmToJSON(clearColor);
			settingsJSON["dimension"] = camera->cameraDimension;
			settingsJSON["limit"] = camera->limit;
			settingsJSON["up"] = glmToJSON(camera->up);

			settingsFile << settingsJSON.dump(4);
			settingsFile.close();
		}
	}

	void loadSettings() {
		std::ifstream settingsFile;
		settingsFile.open("levels\\" + scene->name + "\\settings.json");

		if (settingsFile.is_open()) {
			std::string settingsString((std::istreambuf_iterator<char>(settingsFile)), (std::istreambuf_iterator<char>()));

			json settingsJSON = json::parse(settingsString);

			systems.physics3DSystem->gravity = jsonToBTVector3(settingsJSON["gravity"]);
			systems.physics3DSystem->applySettings();
			camera->radius = settingsJSON["radius"];
			camera->speed2D = settingsJSON["speed2D"];
			camera->speed3D = settingsJSON["speed3D"];
			camera->movementSpeed2D = settingsJSON["movementSpeed2D"];
			camera->movementSpeed3D = settingsJSON["movementSpeed3D"];
			camera->zoom = settingsJSON["zoom"];
			camera->fovy = settingsJSON["fovy"];
			camera->zNear = settingsJSON["zNear"];
			camera->zFar = settingsJSON["zFar"];
			camera->eye = jsonToGLMVec3(settingsJSON["eye"]);
			camera->jumpForce2D = settingsJSON["jumpForce2D"];
			camera->jumpForce3D = settingsJSON["jumpForce3D"];
			camera->cameraDimension = static_cast<CameraDimension>(settingsJSON["dimension"]);
			clearColor = jsonToGLMVec4(settingsJSON["clearColor"]);
			camera->limit = settingsJSON["limit"];
			camera->up = jsonToGLMVec3(settingsJSON["up"]);

			settingsFile.close();
		}
	}

	void displaySettings() {
		if (areSettingsVisble) {
			if (!ImGui::Begin("Settings", &areSettingsCollapsed)) {
				ImGui::End();
				return;
			}

			if (ImGui::TreeNode("General")) {
				ImGui::ColorEdit4("Clear Color", (float*)& clearColor);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Player")) {
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Physics")) {
				ImGui::InputFloat3("Gravity", (float*)& systems.physics3DSystem->gravity);

				if (ImGui::Button("Apply")) {
					systems.physics3DSystem->applySettings();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Camera")) {
				ImGui::SliderFloat("Radius", &camera->radius, 1.0f, 100.0f);
				ImGui::InputFloat("Sensitivity", &camera->sensitivity);
				ImGui::InputFloat("Speed 2D", &camera->speed2D);
				ImGui::InputFloat("Speed 3D", &camera->speed3D);
				ImGui::InputFloat("Movement Speed 2D", &camera->movementSpeed2D);
				ImGui::InputFloat("Movement Speed 3D", &camera->movementSpeed3D);
				ImGui::InputFloat("Zoom", &camera->zoom);
				ImGui::InputFloat("FOV", &camera->fovy);
				ImGui::InputFloat("ZNear", &camera->zNear);
				ImGui::InputFloat("ZFar", &camera->zFar);
				ImGui::InputFloat3("Position", (float*)& camera->eye);
				ImGui::InputFloat("JumpForce2D", &camera->jumpForce2D);
				ImGui::InputFloat("JumpForce3D", &camera->jumpForce3D);
				
				const char* dimensions[2] = { "2D", "3D" };
				ImGui::Combo("Level dimension", (int*)&camera->cameraDimension, dimensions, IM_ARRAYSIZE(dimensions));

				if (ImGui::Button("Detach Target")) {
					camera->detach();
				}

				ImGui::TreePop();
			}

			ImGui::End();
		}
	}

	void displayPhysics2DConf(Physics2DConf & physics2DConf) {
		if (ImGui::TreeNode("Body Configuration")) {
			InputBool("Allow Sleep", &physics2DConf.bodyConf.allowSleep);
			ImGui::InputFloat("Angle", (float*)& physics2DConf.bodyConf.angle);
			ImGui::InputFloat("Angular Acceleration", (float*)& physics2DConf.bodyConf.angularAcceleration);
			ImGui::SliderFloat("Angular Damping", (float*)& physics2DConf.bodyConf.angularDamping, 0.0f, 1.0f);
			ImGui::InputFloat("Angular Velocity", (float*)& physics2DConf.bodyConf.angularVelocity);
			InputBool("Awake", &physics2DConf.bodyConf.awake);
			InputBool("Enabled", &physics2DConf.bodyConf.enabled);
			InputBool("Fixed Rotation", &physics2DConf.bodyConf.fixedRotation);
			ImGui::InputFloat2("Linear Acceleration", (float*)& physics2DConf.bodyConf.linearAcceleration);
			ImGui::SliderFloat("LinearDamping", (float*)& physics2DConf.bodyConf.linearDamping, 0.0f, 1.0f);
			ImGui::InputFloat2("Linear Velocity", (float*)& physics2DConf.bodyConf.linearVelocity);
			ImGui::InputFloat2("Position", (float*)& physics2DConf.bodyConf.location);
			ImGui::Combo("Type", (int*)&physics2DConf.bodyConf.type, physics2DConf.types, IM_ARRAYSIZE(physics2DConf.types));

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Shape Configuration")) {
			static int shape = 0;
			ImGui::Combo("Shape", &shape, physics2DConf.shapes, IM_ARRAYSIZE(physics2DConf.shapes));
			physics2DConf.shape = static_cast<Shape2D>(shape);

			ImGui::InputFloat("Density", &physics2DConf.density);
			ImGui::SliderFloat("Friction", &physics2DConf.friction, 0.0f, 1.0f);
			ImGui::SliderFloat("Restitution", &physics2DConf.restitution, 0.0f, 1.0f);
			ImGui::InputFloat("Radius", &physics2DConf.radius);

			if (physics2DConf.shape == Shape2D::POLYGON_SHAPE) {
				InputBool("Set As Box", &physics2DConf.setAsBox);

				if (physics2DConf.setAsBox) {
					ImGui::InputFloat2("Scaling", (float*)& physics2DConf.scaling);
				}
			}

			if (!physics2DConf.setAsBox && physics2DConf.shape != Shape2D::DISK_SHAPE) {
				ImGui::Text("Vertices");

				for (int a = 0; a < physics2DConf.vertices.size(); a++) {
					ImGui::InputFloat2(("Vertex " + std::to_string(a + 1)).c_str(), (float*)& physics2DConf.vertices[a]);
				}

				if (ImGui::Button("Add Vertex")) {
					physics2DConf.vertices.push_back(pr::Vec2(0.0f, 0.0f));
				}
			}

			ImGui::TreePop();
		}
	}

	void displayPhysics3DConf(Physics3DConf & physics3DConf) {
		ImGui::InputFloat("Mass", &physics3DConf.mass);

		static int shape = 0;
		const char* shapes[2] = { "BOX", "SPHERE" };

		ImGui::Combo("Shape", &shape, shapes, 2);
		physics3DConf.shape = static_cast<Shape3D>(shape);

		if (physics3DConf.shape == Shape3D::BOX) {
			ImGui::InputFloat3("Scaling", (float*)& physics3DConf.scaling);
		}
		else if (physics3DConf.shape == Shape3D::SPHERE) {
			ImGui::InputFloat("Radius", &physics3DConf.radius);
		}

		ImGui::InputFloat3("Origin", (float*)& physics3DConf.origin);
		ImGui::InputFloat3("Rotation", (float*)& physics3DConf.rotation);
		ImGui::SliderFloat("Angular Damping", &physics3DConf.angularDamping, 0.0f, 1.0f);
		ImGui::InputFloat("Angular Sleeping Threshold", &physics3DConf.angularSleepingThreshold);
		ImGui::SliderFloat("Friction", &physics3DConf.friction, 0.0f, 1.0f);
		ImGui::SliderFloat("Linear Damping", &physics3DConf.linearDamping, 0.0f, 1.0f);
		ImGui::InputFloat("Linear Sleeping Threshold", &physics3DConf.linearSleepingThreshold);
		ImGui::SliderFloat("Restitution", &physics3DConf.restitution, 0.0f, 1.0f);
		ImGui::SliderFloat("Rolling Friction", &physics3DConf.rollingFriction, 0.0f, 1.0f);
		ImGui::SliderFloat("Spinning Friction", &physics3DConf.spinningFriction, 0.0f, 1.0f);
		ImGui::InputInt("User Index", &physics3DConf.userIndex);
		ImGui::Text("Set user index to 1 to mark this as START OBJECT");
		ImGui::Text("Set user index to 2 to mark this as END OBJECT");
	}

	void displayNoiseConf(NoiseConf & noiseConf) {
		ImGui::Combo("Noise Type", (int*)& noiseConf.m_noiseType, NoiseConf::noiseTypes, IM_ARRAYSIZE(NoiseConf::noiseTypes));
		ImGui::Combo("Fractal Type", (int*)& noiseConf.m_fractalType, NoiseConf::fractalTypes, IM_ARRAYSIZE(NoiseConf::fractalTypes));
		ImGui::Combo("Cellular Return Type", (int*)& noiseConf.m_cellularReturnType, NoiseConf::cellularReturnTypes, IM_ARRAYSIZE(NoiseConf::cellularReturnTypes));
		ImGui::Combo("Cellular Distance Function", (int*)& noiseConf.m_cellularDistanceFunction, NoiseConf::cellularDistanceFunctions, IM_ARRAYSIZE(NoiseConf::cellularDistanceFunctions));
		ImGui::Combo("Interp", (int*)& noiseConf.m_interp, NoiseConf::interps, IM_ARRAYSIZE(NoiseConf::interps));
		ImGui::InputInt("Cellular Distance Index 0", &noiseConf.m_cellularDistanceIndex0);
		ImGui::InputInt("Cellular Distance Index 1", &noiseConf.m_cellularDistanceIndex1);
		ImGui::InputFloat("Cellular Jitter", &noiseConf.m_cellularJitter);
		ImGui::InputFloat("Fractal Bounding", &noiseConf.m_fractalBounding);
		ImGui::InputFloat("Frequency", &noiseConf.m_frequency);
		ImGui::InputFloat("Gain", &noiseConf.m_gain);
		ImGui::InputFloat("Gradient Perturb Amp", &noiseConf.m_gradientPerturbAmp);
		ImGui::InputFloat("Lacunarity", &noiseConf.m_lacunarity);
		ImGui::InputInt("Octaves", &noiseConf.m_octaves);
		ImGui::InputInt("Seed", &noiseConf.m_seed);
		ImGui::InputFloat("Amplitude", &noiseConf.amplitude);
	}

	void displayRandomizer() {
		if (isRandomizerVisible) {
			if (!ImGui::Begin("Randomizer", &isRandomizerCollapsed)) {
				ImGui::End();
				return;
			}

			TransformSystem* transformSystem = systems.transformSystem;
			TransformComponent& parentTransform = registry->get<TransformComponent>(selectedEntity);
			glm::vec3 parentPosition = parentTransform.getPosition();

			static RandomizerConf randomizerConf;
			static Physics2DConf physics2DConf;
			static Physics3DConf physics3DConf;

			if (ImGui::TreeNode("Random Segment")) {
				static NoiseConf noiseConf;

				if (ImGui::TreeNode("General")) {
					static int dimension = 0;

					ImGui::Combo("Dimension", &dimension, randomizerConf.dimensions, IM_ARRAYSIZE(randomizerConf.dimensions));
					randomizerConf.dimension = static_cast<Dimension>(dimension);

					static int pattern = 0;

					ImGui::Combo("Pattern", &pattern, randomizerConf.patterns, IM_ARRAYSIZE(randomizerConf.patterns));
					randomizerConf.pattern = static_cast<Pattern>(pattern);
					ImGui::SliderInt("Object Count Min", &randomizerConf.objectCountMin, 0, 100);
					ImGui::SliderInt("Object Count Max", &randomizerConf.objectCountMax, 0, 100);
					ImGui::InputFloat3("Position Min", (float*)& randomizerConf.positionMin);
					ImGui::InputFloat3("Position Max", (float*)& randomizerConf.positionMax);
					ImGui::InputFloat3("Scale Min", (float*)& randomizerConf.scaleMin);
					ImGui::InputFloat3("Scale Max", (float*)& randomizerConf.scaleMax);
					ImGui::InputFloat3("Rotation Min", (float*)& randomizerConf.rotationMin);
					ImGui::InputFloat3("Rotation Max", (float*)& randomizerConf.rotationMax);
					ImGui::ColorEdit4("Color Min", (float*)& randomizerConf.colorMin);
					ImGui::ColorEdit4("Color Max", (float*)& randomizerConf.colorMax);
					InputBool("Use Physics", &randomizerConf.usePhysics);

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Pattern")) {
					if (randomizerConf.pattern == Pattern::GRID) {
						ImGui::InputInt2("Grid", (int*)& randomizerConf.grid);
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Noise")) {

					displayNoiseConf(noiseConf);

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Physics")) {
					if (randomizerConf.dimension == Dimension::D3) {
						displayPhysics3DConf(physics3DConf);
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Texture")) {
					static std::string texture = randomizerConf.textureName;

					ImGui::InputText("Texture", &texture, ImGuiInputTextFlags_ReadOnly);

					if (ImGui::TreeNode("Textures")) {
						for (auto tex : textures) {
							if (ImGui::Selectable(tex.first.c_str(), tex.first == texture)) {
								texture = tex.first;
							}
						}

						ImGui::TreePop();
					}

					ImGui::Spacing();

					if (ImGui::Button("Apply Texture")) {
						if (textures.find(texture) != textures.end()) {
							randomizerConf.texture = textures[texture];
							randomizerConf.textureName = texture;
						}
					}

					ImGui::TreePop();
				}

				if (ImGui::Button("Randomize")) {
					randomizerConf.texture = textures[randomizerConf.textureName];
					transformSystem->clearChildren(selectedEntity);

					std::vector<glm::vec3> positions;
					std::vector<glm::vec3> rotations;
					std::vector<glm::vec3> scales;
					std::vector<glm::vec4> colors;

					int objectCount = glm::linearRand(randomizerConf.objectCountMin, randomizerConf.objectCountMax);

					if (randomizerConf.pattern == Pattern::CUSTOM) {
						for (int a = 0; a < objectCount; a++) {
							glm::vec3 position = parentPosition + glm::linearRand(randomizerConf.positionMin, randomizerConf.positionMax);
							glm::vec3 scale = glm::linearRand(randomizerConf.scaleMin, randomizerConf.scaleMax);
							glm::vec3 rotation = glm::linearRand(randomizerConf.rotationMin, randomizerConf.rotationMax);
							glm::vec4 color = glm::linearRand(randomizerConf.colorMin, randomizerConf.colorMax);

							positions.push_back(position);
							rotations.push_back(rotation);
							scales.push_back(scale);
							colors.push_back(color);
						}
					}
					else if (randomizerConf.pattern == Pattern::GRID) {
						glm::vec3 offset = glm::vec3(
							randomizerConf.grid.x * randomizerConf.scaleMax.x / 2.0f,
							0.0f,
							randomizerConf.grid.y * randomizerConf.scaleMax.z / 2.0f
						);

						objectCount = randomizerConf.grid.x * randomizerConf.grid.y;

						for (int x = 0; x < randomizerConf.grid.x; x++) {
							for (int z = 0; z < randomizerConf.grid.y; z++) {
								glm::vec3 position = glm::vec3(x * randomizerConf.scaleMax.x, 0.0f, z * randomizerConf.scaleMax.z) - offset;
								glm::vec3 scale = glm::linearRand(randomizerConf.scaleMin, randomizerConf.scaleMax);
								scale.y = glm::abs(noiseConf.GetNoise(x, z)) * noiseConf.amplitude;

								positions.push_back(position);
								rotations.push_back(glm::vec3(0.0f));
								scales.push_back(scale);
								colors.push_back(glm::linearRand(randomizerConf.colorMin, randomizerConf.colorMax));
							}
						}
					}

					for (int a = 0; a < objectCount; a++) {
						uint32_t child = registry->create();
						registry->assign<TransformComponent>(child, "Entity ", positions[a], rotations[a], scales[a]);
						registry->assign<CuboidComponent>(child, colors[a]);
						CuboidComponent& cuboidComponent = registry->get<CuboidComponent>(child);
						cuboidComponent.setTexture(randomizerConf.textureName, randomizerConf.texture);

						TransformComponent& transform = registry->get<TransformComponent>(child);
						transformSystem->appendChild(selectedEntity, child);
						transform.name += transform.id;

						if (randomizerConf.usePhysics) {
							physics3DConf.origin = btVector3(positions[a].x, positions[a].y, positions[a].z);
							physics3DConf.rotation = btVector3(rotations[a].x, rotations[a].y, rotations[a].z);
							physics3DConf.scaling = btVector3(scales[a].x, scales[a].y, scales[a].z);
							registry->assign<Physics3DComponent>(child, *systems.physics3DSystem->getWorld(), physics3DConf);
							savePhysics3DConf(transform.id, physics3DConf);
						}
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Random Texture")) {
				static std::string textureName = "";
				static GLuint texture;
				static TextureConf textureConf;


				ImGui::InputText("Name", &textureConf.name);
				ImGui::SliderInt("Width", &textureConf.width, 0, 128);
				ImGui::SliderInt("Height", &textureConf.height, 0, 128);

				static int textureType = 0;
				ImGui::Combo("Texture Type", &textureType, textureConf.textureTypes, IM_ARRAYSIZE(textureConf.textureTypes));
				textureConf.textureType = static_cast<TextureType>(textureType);

				if (textureConf.textureType == TextureType::NOISE) {
					displayNoiseConf(textureConf.noiseConf);
					ImGui::SliderFloat3("Multiplier", (float*)& textureConf.multiplier, 0.0f, 1.0f);
				}

				if (textureConf.textureType == TextureType::MONOCHROMATIC) {
					ImGui::ColorEdit3("Color", (float*)& textureConf.color);
				}

				if (ImGui::TreeNode("Textures")) {
					for (auto tex : textures) {
						if (ImGui::Selectable(tex.first.c_str(), tex.first == textureName)) {
							textureName = tex.first;
							texture = tex.second;
						}
					}

					ImGui::TreePop();
				}

				if (ImGui::Button("Generate Texture")) {
					if (!(textures.find(textureName) != textures.end())) {
						if (textureConf.name != "") {
							GLuint texture;
							glGenTextures(1, &texture);
							glBindTexture(GL_TEXTURE_2D, texture);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

							textures.insert(std::pair(textureConf.name, texture));

							unsigned char* data = generateTexture(textureConf);

							stbi_write_png(("levels\\" + textureConf.name + ".png").c_str(), textureConf.width, textureConf.height, 3, data, 3 * sizeof(char) * textureConf.width);

							std::ifstream rTextureListFile;
							rTextureListFile.open("levels\\textureList.json");

							json textureListObject;

							if (rTextureListFile.is_open()) {
								std::string textureListString((std::istreambuf_iterator<char>(rTextureListFile)), (std::istreambuf_iterator<char>()));

								textureListObject = json::parse(textureListString);
							}

							textureListObject.push_back(textureConf.name);

							std::ofstream wTextureListFile;
							wTextureListFile.open("levels\\textureList.json", std::fstream::trunc);

							if (wTextureListFile.is_open()) {
								wTextureListFile << textureListObject.dump(4);

								wTextureListFile.close();
							}

							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureConf.width, textureConf.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
						}
					}
				}

				ImGui::TreePop();
			}

			ImGui::End();
		}
	}

	void savePhysics2DConf(std::string id, Physics2DConf & physics2DConf) {
		std::ifstream rPhysicsConfsFile;
		rPhysicsConfsFile.open("levels\\" + scene->name + "\\physicsConfs.json");

		json physicsConfsJSON;

		if (rPhysicsConfsFile.is_open()) {
			std::string physicsConfsString((std::istreambuf_iterator<char>(rPhysicsConfsFile)), (std::istreambuf_iterator<char>()));
			physicsConfsJSON = json::parse(physicsConfsString);
			rPhysicsConfsFile.close();
		}

		std::ofstream wPhysicsConfsFile;
		wPhysicsConfsFile.open("levels\\" + scene->name + "\\physicsConfs.json", std::fstream::trunc);

		if (wPhysicsConfsFile.is_open()) {
			json physicsConfObject;

			if (physicsConfsJSON.contains(id)) {
				physicsConfObject = physicsConfsJSON[id];
			}

			physicsConfObject["allowSleep"] = physics2DConf.bodyConf.allowSleep;
			physicsConfObject["angle"] = glm::degrees(physics2DConf.bodyConf.angle);
			physicsConfObject["angularAcceleration"] = physics2DConf.bodyConf.angularAcceleration;
			physicsConfObject["angularDamping"] = (float)physics2DConf.bodyConf.angularDamping;
			physicsConfObject["angularVelocity"] = physics2DConf.bodyConf.angularVelocity;
			physicsConfObject["awake"] = physics2DConf.bodyConf.awake;
			physicsConfObject["enabled"] = physics2DConf.bodyConf.enabled;
			physicsConfObject["fixedRotation"] = physics2DConf.bodyConf.fixedRotation;
			physicsConfObject["linearAcceleration"] = json(physics2DConf.bodyConf.linearAcceleration);
			physicsConfObject["linearDamping"] = (float)physics2DConf.bodyConf.linearDamping;
			physicsConfObject["linearVelocity"] = json(physics2DConf.bodyConf.linearVelocity);
			physicsConfObject["location"] = json(physics2DConf.bodyConf.location);
			physicsConfObject["type"] = (int)physics2DConf.bodyConf.type;
			physicsConfObject["density"] = physics2DConf.density;
			physicsConfObject["friction"] = physics2DConf.friction;
			physicsConfObject["setAsBox"] = physics2DConf.setAsBox;
			physicsConfObject["shape"] = physics2DConf.shape;
			physicsConfObject["scaling"] = json(physics2DConf.scaling);
			physicsConfObject["radius"] = physics2DConf.radius;
			physicsConfObject["vertices"] = json(physics2DConf.vertices);

			physicsConfsJSON[id] = physicsConfObject;

			wPhysicsConfsFile << physicsConfsJSON.dump(4);
			wPhysicsConfsFile.close();
		}
	}

	void savePhysics3DConf(std::string id, Physics3DConf & physics3DConf) {
		std::ifstream rPhysicsConfsFile;
		rPhysicsConfsFile.open("levels\\" + scene->name + "\\physicsConfs.json");

		json physicsConfsJSON;

		if (rPhysicsConfsFile.is_open()) {
			std::string physicsConfsString((std::istreambuf_iterator<char>(rPhysicsConfsFile)), (std::istreambuf_iterator<char>()));
			physicsConfsJSON = json::parse(physicsConfsString);
			rPhysicsConfsFile.close();
		}

		std::ofstream wPhysicsConfsFile;
		wPhysicsConfsFile.open("levels\\" + scene->name + "\\physicsConfs.json", std::fstream::trunc);

		if (wPhysicsConfsFile.is_open()) {
			json physicsConfObject;

			if (physicsConfsJSON.contains(id)) {
				physicsConfObject = physicsConfsJSON[id];
			}

			physicsConfObject["dimension"] = "3D";
			physicsConfObject["mass"] = physics3DConf.mass;
			physicsConfObject["shape"] = physics3DConf.shape;
			physicsConfObject["scaling"] = btToJSON(physics3DConf.scaling);
			physicsConfObject["radius"] = physics3DConf.radius;
			physicsConfObject["origin"] = btToJSON(physics3DConf.origin);
			physicsConfObject["rotation"] = btToJSON(physics3DConf.rotation);
			physicsConfObject["angularDamping"] = physics3DConf.angularDamping;
			physicsConfObject["angularSleepingThreshold"] = physics3DConf.angularSleepingThreshold;
			physicsConfObject["friction"] = physics3DConf.friction;
			physicsConfObject["linearDamping"] = physics3DConf.linearDamping;
			physicsConfObject["linearSleepingThreshold"] = physics3DConf.linearSleepingThreshold;
			physicsConfObject["restitution"] = physics3DConf.restitution;
			physicsConfObject["rollingFriction"] = physics3DConf.rollingFriction;
			physicsConfObject["spinningFriction"] = physics3DConf.spinningFriction;
			physicsConfObject["userIndex"] = physics3DConf.userIndex;
			physicsConfsJSON[id] = physicsConfObject;

			wPhysicsConfsFile << physicsConfsJSON.dump(4);
			wPhysicsConfsFile.close();
		}
	}

	template <class Component>
	void copyPaste(Component& comp0, Component& comp1) {
		if (ImGui::Button("Copy")) {
			comp0 = comp1;
		}

		if (ImGui::Button("Paste")) {
			comp1 = comp0;
		}
	}

	void displayEntityComponentView() {
		if (isEntityComponentViewVisible) {
			if (!ImGui::Begin("Entity Component View", &isEntityComponentViewCollapsed)) {
				ImGui::End();
				return;
			}

			pr2D::World* world2D = systems.physics2DSystem->getWorld();
			btDiscreteDynamicsWorld* world3D = systems.physics3DSystem->getWorld();

			bool hasTransform = registry->has<TransformComponent>(selectedEntity);
			bool hasPhysics2D = registry->has<Physics2DComponent>(selectedEntity);
			bool hasPhysics3D = registry->has<Physics3DComponent>(selectedEntity);
			bool hasPoint = registry->has<PointComponent>(selectedEntity);
			bool hasLine = registry->has<LineComponent>(selectedEntity);
			bool hasShape = registry->has<ShapeComponent>(selectedEntity);
			bool hasRectangle = registry->has<RectangleComponent>(selectedEntity);
			bool hasCuboid = registry->has<CuboidComponent>(selectedEntity);
			bool hasCircle = registry->has<CircleComponent>(selectedEntity);
			bool hasSphere = registry->has<SphereComponent>(selectedEntity);
			bool hasPhysics2DConf = registry->has<Physics2DConf>(selectedEntity);
			bool hasPhysics3DConf = registry->has<Physics3DConf>(selectedEntity);

			std::string id;

			if (hasTransform) {
				TransformComponent& transform = registry->get<TransformComponent>(selectedEntity);
				id = transform.getID();

				if (transform.isDestroyed) {
					registry->destroy(selectedEntity);
					selectedEntity = 0;
					ImGui::End();
					return;
				}

				if (ImGui::TreeNode("Transform")) {
					ImGui::InputText("ID", &transform.id, ImGuiInputTextFlags_ReadOnly);
					ImGui::InputText("Name", &transform.name);
					ImGui::InputFloat3("Position", (float*)& transform.position);
					ImGui::InputFloat3("Rotation", (float*)& transform.rotation);
					ImGui::InputFloat3("Scale", (float*)& transform.scale);

					if (ImGui::Button("Clear Children")) {
						systems.transformSystem->clearChildren(selectedEntity);
					}

					if (selectedEntity) {
						if (ImGui::Button("Destroy")) {
							systems.transformSystem->destroyChild(selectedEntity);
						}
					}

					ImGui::TreePop();
				}
			}
			else {
				ImGui::End();
				return;
			}

			bool shouldShowWarning = false;
			static std::string warningText = "";

			if (hasPhysics2D) {
				if (ImGui::TreeNode("Physics2D")) {


					Physics2DComponent& physics2D = registry->get<Physics2DComponent>(selectedEntity);
					pr2D::Body* body = physics2D.getBody();
					ImGui::InputFloat2("Location", (float*)& body->GetLocation(), ImGuiInputTextFlags_ReadOnly);

					//copyPaste(copyBuffer.physics2D, physics2D);

					if (ImGui::Button("Set as target")) {
						camera->setTarget(physics2D);
					}

					if (ImGui::Button("Destroy")) {
						physics2D.getBody()->Destroy(physics2D.getFixture());
						registry->remove<Physics2DComponent>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (hasPhysics2DConf) {
				Physics2DConf& physics2DConf = registry->get<Physics2DConf>(selectedEntity);
				TransformComponent& transform = registry->get<TransformComponent>(selectedEntity);

				transform.position = glm::vec3(physics2DConf.bodyConf.location[0], physics2DConf.bodyConf.location[1], 0.0f);
				transform.rotation = glm::vec3(0.0f, physics2DConf.bodyConf.angle, 0.0f);

				if (physics2DConf.setAsBox) {
					transform.scale = glm::vec3(physics2DConf.scaling[0], physics2DConf.scaling[1], 1.0f);
				}
				else if (physics2DConf.shape == Shape2D::DISK_SHAPE) {
					transform.scale = glm::vec3(physics2DConf.radius, physics2DConf.radius, 0.0f);
				}

				//copyPaste(copyBuffer.physics2DConf, physics2DConf);

				if (ImGui::TreeNode("Physics2D Configuration")) {
					displayPhysics2DConf(physics2DConf);

					if (ImGui::Button("Init")) {
						savePhysics2DConf(transform.getID(), physics2DConf);

						registry->assign<Physics2DComponent>(selectedEntity, *world2D, physics2DConf);
						registry->remove<Physics2DConf>(selectedEntity);
					}

					if (ImGui::Button("Destroy")) {
						registry->remove<Physics2DConf>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (hasPhysics3D) {
				if (ImGui::TreeNode("Physics3D Component")) {
					Physics3DComponent& physics3D = registry->get<Physics3DComponent>(selectedEntity);

					const char* actions[] = {
						"Central Force (vector)",
						"Damping (scalar, scalar)",
						"Force (vector, vector)",
						"Impulse (vector, vector)",
						"Torque (vector)",
						"Torque Impulse (vector)",
						"Spinning Friction (scalar)",
						"Sleeping Thresholds (scalar, scalar)",
						"Rolling Friction (scalar)",
						"Restitution (scalar)",
						"Linear Velocity (vector)",
						"Angular Velocity (vector)",
						"Friction (scalar)"
					};

					static int action;
					ImGui::Combo("Action", &action, actions, IM_ARRAYSIZE(actions));

					static btVector3 vector(0.0f, 0.0f, 0.0f);
					static btVector3 vector1(0.0f, 0.0f, 0.0f);
					static btScalar scalar = 0.0f;
					static btScalar scalar1 = 0.0f;

					ImGui::InputFloat("Scalar", &scalar);
					ImGui::InputFloat("Scalar 1", &scalar1);
					ImGui::InputFloat3("Vector", (float*)& vector);
					ImGui::InputFloat3("Vector 1", (float*)& vector1);

					if (ImGui::Button("Apply")) {
						switch (action) {
						case 0: physics3D.rigidBody->applyCentralForce(vector); break;
						case 1: physics3D.rigidBody->setDamping(scalar, scalar1); break;
						case 2: physics3D.rigidBody->applyForce(vector, vector1); break;
						case 3: physics3D.rigidBody->applyImpulse(vector, vector1); break;
						case 4: physics3D.rigidBody->applyTorque(vector); break;
						case 5: physics3D.rigidBody->applyTorqueImpulse(vector); break;
						case 6: physics3D.rigidBody->setSpinningFriction(scalar); break;
						case 7: physics3D.rigidBody->setSleepingThresholds(scalar, scalar1); break;
						case 8: physics3D.rigidBody->setRollingFriction(scalar); break;
						case 9: physics3D.rigidBody->setRestitution(scalar); break;
						case 10: physics3D.rigidBody->setLinearVelocity(vector); break;
						case 11: physics3D.rigidBody->setAngularVelocity(vector); break;
						case 12: physics3D.rigidBody->setFriction(scalar); break;
						}
					}

					//copyPaste(copyBuffer.physics3D, physics3D);

					if (ImGui::Button("Set As Target")) {
						camera->setTarget(physics3D);
					}

					if (ImGui::Button("Destroy")) {
						systems.physics3DSystem->world3D->removeCollisionObject(physics3D.getRigidBody());
						registry->remove<Physics3DComponent>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (hasPhysics3DConf) {
				TransformComponent& transform = registry->get<TransformComponent>(selectedEntity);
				Physics3DConf& physics3DConf = registry->get<Physics3DConf>(selectedEntity);

				transform.position = glm::vec3(physics3DConf.origin.getX(), physics3DConf.origin.getY(), physics3DConf.origin.getZ());
				transform.rotation = glm::vec3(physics3DConf.rotation.getX(), physics3DConf.rotation.getZ(), physics3DConf.rotation.getY());

				if (physics3DConf.shape == Shape3D::BOX) {
					transform.scale = glm::vec3(physics3DConf.scaling.getX(), physics3DConf.scaling.getY(), physics3DConf.scaling.getZ());
				}
				else {
					transform.scale = glm::vec3(physics3DConf.radius);
				}

				if (ImGui::TreeNode("Physics3D Configuration")) {
					//copyPaste(copyBuffer.physics3DConf, physics3DConf);

					displayPhysics3DConf(physics3DConf);

					if (ImGui::Button("Init")) {
						savePhysics3DConf(transform.id, physics3DConf);

						registry->assign<Physics3DComponent>(selectedEntity, *world3D, physics3DConf);
						registry->remove<Physics3DConf>(selectedEntity);
					}

					if (ImGui::Button("Destroy")) {
						registry->remove<Physics3DConf>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (hasPoint) {
				if (ImGui::TreeNode("Point Component")) {
					PointComponent& point = registry->get<PointComponent>(selectedEntity);
					std::vector<glm::vec3>& vertices = point.getVertices();
					std::vector<glm::vec4>& colors = point.getColors();

					//copyPaste(copyBuffer.point, point);

					if (ImGui::Button("Add vertex")) {
						vertices.push_back(glm::vec3(0.0f));
						colors.push_back(glm::vec4(1.0f));
					}

					ImGui::Columns(2);

					for (int a = 0; a < vertices.size(); a++) {
						ImGui::InputFloat3(("Vertex " + std::to_string(a + 1)).c_str(), (float*)& vertices[a]);
					}

					ImGui::NextColumn();

					for (int a = 0; a < colors.size(); a++) {
						ImGui::ColorEdit4(("Color " + std::to_string(a + 1)).c_str(), (float*)& colors[a]);
					}

					ImGui::Columns(1);


					if (ImGui::Button("Destroy")) {
						registry->remove<PointComponent>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (hasLine) {
				if (ImGui::TreeNode("LineComponent")) {
					LineComponent& line = registry->get<LineComponent>(selectedEntity);
					std::vector<glm::vec3>& vertices = line.getVertices();
					std::vector<glm::vec4>& colors = line.getColors();

					//copyPaste(copyBuffer.line, line);

					if (ImGui::Button("Add vertex")) {
						vertices.push_back(glm::vec3(0.0f));
						colors.push_back(glm::vec4(1.0f));
					}

					ImGui::Columns(2);

					for (int a = 0; a < vertices.size(); a++) {
						ImGui::InputFloat3(("Vertex " + std::to_string(a + 1)).c_str(), (float*)& vertices[a]);
					}

					ImGui::NextColumn();

					for (int a = 0; a < colors.size(); a++) {
						ImGui::ColorEdit4(("Color " + std::to_string(a + 1)).c_str(), (float*)& colors[a]);
					}

					ImGui::Columns(1);

					if (ImGui::Button("Destroy")) {
						registry->remove<LineComponent>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (hasShape) {
				if (ImGui::TreeNode("Shape Component")) {
					auto& shape = registry->get<ShapeComponent>(selectedEntity);

					//copyPaste(copyBuffer.shape, shape);

					std::vector<glm::vec3>& vertices = shape.getVertices();
					std::vector<glm::uvec3>& indices = shape.getIndices();
					std::vector<glm::vec4>& colors = shape.getColors();

					ImGui::Text("Use Indices");

					if (ImGui::RadioButton("True", shape.isIndexed())) {
						shape.setIndexed(true);
					}

					ImGui::SameLine();

					if (ImGui::RadioButton("False", !shape.isIndexed())) {
						shape.setIndexed(false);
					}

					if (ImGui::Button("Add vertex")) {
						vertices.push_back(glm::vec3(0.0f));
						colors.push_back(glm::vec4(1.0f));
					}

					ImGui::Columns(2);

					for (int a = 0; a < vertices.size(); a++) {
						ImGui::InputFloat3(("Vertex " + std::to_string(a + 1)).c_str(), (float*)& vertices[a]);
					}

					ImGui::NextColumn();

					for (int a = 0; a < colors.size(); a++) {
						ImGui::ColorEdit4(("Color " + std::to_string(a + 1)).c_str(), (float*)& colors[a]);
					}

					ImGui::Columns(1);

					if (ImGui::Button("Add triangle")) {
						indices.push_back(glm::uvec3(0));
					}

					for (int a = 0; a < indices.size(); a++) {
						ImGui::InputInt3(("Triangle " + std::to_string(a + 1)).c_str(), (int*)& indices[a]);
					}

					if (ImGui::Button("Destroy")) {
						registry->remove<ShapeComponent>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (hasRectangle) {
				if (ImGui::TreeNode("Rectangle Component")) {
					auto& rectangle = registry->get<RectangleComponent>(selectedEntity);

					//copyPaste(copyBuffer.rectangle, rectangle);

					ImGui::ColorEdit4("Color", (float*)& rectangle.getColor());

					static std::string texture = rectangle.getTextureName();

					ImGui::InputText("Texture", &texture, ImGuiInputTextFlags_ReadOnly);

					if (ImGui::TreeNode("Textures")) {
						for (auto tex : textures) {
							if (ImGui::Selectable(tex.first.c_str(), tex.first == texture)) {
								texture = tex.first;
							}
						}

						ImGui::TreePop();
					}

					ImGui::Spacing();

					if (ImGui::Button("Apply Texture")) {
						if (textures.find(texture) != textures.end()) {
							rectangle.setTexture(texture, textures[texture]);
						}
					}

					if (ImGui::Button("Destroy")) {
						registry->remove<RectangleComponent>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (hasCircle) {
				if (ImGui::TreeNode("Circle Component")) {
					auto& circle = registry->get<CircleComponent>(selectedEntity);
					//copyPaste(copyBuffer.circle, circle);

					ImGui::ColorEdit4("Color", (float*)& circle.getColor());

					static std::string texture = circle.getTextureName();

					ImGui::InputText("Texture", &texture, ImGuiInputTextFlags_ReadOnly);

					if (ImGui::TreeNode("Textures")) {
						for (auto tex : textures) {
							if (ImGui::Selectable(tex.first.c_str(), tex.first == texture)) {
								texture = tex.first;
							}
						}

						ImGui::TreePop();
					}

					ImGui::Spacing();

					if (ImGui::Button("Apply Texture")) {
						if (textures.find(texture) != textures.end()) {
							circle.setTexture(texture, textures[texture]);
						}
					}

					if (ImGui::Button("Destroy")) {
						registry->remove<CircleComponent>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (hasCuboid) {
				if (ImGui::TreeNode("Cuboid Component")) {
					auto& cuboid = registry->get<CuboidComponent>(selectedEntity);
					//copyPaste(copyBuffer.cuboid, cuboid);

					ImGui::ColorEdit4("Color", (float*)& cuboid.getColor());

					static std::string texture = cuboid.getTextureName();

					ImGui::InputText("Texture", &texture, ImGuiInputTextFlags_ReadOnly);

					if (ImGui::TreeNode("Textures")) {
						for (auto tex : textures) {
							if (ImGui::Selectable(tex.first.c_str(), tex.first == texture)) {
								texture = tex.first;
							}
						}

						ImGui::TreePop();
					}

					ImGui::Spacing();

					if (ImGui::Button("Apply Texture")) {
						if (textures.find(texture) != textures.end()) {
							cuboid.setTexture(texture, textures[texture]);
						}
					}

					if (ImGui::Button("Destroy")) {
						registry->remove<CuboidComponent>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (hasSphere) {
				if (ImGui::TreeNode("Sphere Component")) {
					auto& sphere = registry->get<SphereComponent>(selectedEntity);
					//copyPaste(copyBuffer.sphere, sphere);
					ImGui::ColorEdit4("Color", (float*)& sphere.getColor());

					static std::string texture = sphere.getTextureName();

					ImGui::InputText("Texture", &texture, ImGuiInputTextFlags_ReadOnly);

					if (ImGui::TreeNode("Textures")) {
						for (auto tex : textures) {
							if (ImGui::Selectable(tex.first.c_str(), tex.first == texture)) {
								texture = tex.first;
							}
						}

						ImGui::TreePop();
					}

					ImGui::Spacing();

					if (ImGui::Button("Apply Texture")) {
						if (textures.find(texture) != textures.end()) {
							sphere.setTexture(texture, textures[texture]);
						}
					}

					if (ImGui::Button("Destroy")) {
						registry->remove<SphereComponent>(selectedEntity);
					}

					ImGui::TreePop();
				}
			}

			if (selectedEntity != 0) {
				if (ImGui::BeginMenu("Add Component")) {
					if (ImGui::MenuItem("Physics2D Component")) {
						if (hasPhysics2D || hasPhysics3D) {
							shouldShowWarning = true;
							warningText = "Entity already has Physics Component!";
						}
						else {
							registry->assign<Physics2DConf>(selectedEntity);
						}
					}

					if (ImGui::MenuItem("Physics3D Component")) {
						if (hasPhysics2D || hasPhysics3D) {
							shouldShowWarning = true;
							warningText = "Entity already has Physics Component!";
						}
						else {
							registry->assign<Physics3DConf>(selectedEntity);
						}
					}

					if (ImGui::MenuItem("Point Component")) {
						if (hasPoint) {
							shouldShowWarning = true;
							warningText = "Entity already has Point Component!";
						}
						else {
							registry->assign<PointComponent>(selectedEntity);
						}
					}

					if (ImGui::MenuItem("Line Component")) {
						if (hasLine) {
							shouldShowWarning = true;
							warningText = "Entity already has Line Component!";
						}
						else {
							registry->assign<LineComponent>(selectedEntity);
						}
					}

					if (ImGui::MenuItem("Shape Component")) {
						if (hasShape) {
							shouldShowWarning = true;
							warningText = "Entity already has Polygon Component!";
						}
						else {
							registry->assign<ShapeComponent>(selectedEntity);
						}
					}

					if (ImGui::MenuItem("Rectangle Component")) {
						if (hasRectangle) {
							shouldShowWarning = true;
							warningText = "Entity already has Rectangle Component!";
						}
						else {
							registry->assign<RectangleComponent>(selectedEntity);
						}
					}

					if (ImGui::MenuItem("Circle Component")) {
						if (hasCircle) {
							shouldShowWarning = true;
							warningText = "Entity already has Circle Component!";
						}
						else {
							registry->assign<CircleComponent>(selectedEntity);
						}
					}

					if (ImGui::MenuItem("Cuboid Component")) {
						if (hasCuboid) {
							shouldShowWarning = true;
							warningText = "Entity already has Cuboid Component!";
						}
						else {
							registry->assign<CuboidComponent>(selectedEntity);
						}
					}

					if (ImGui::MenuItem("Sphere Component")) {
						if (hasSphere) {
							shouldShowWarning = true;
							warningText = "Entity already has Sphere Component!";
						}
						else {
							registry->assign<SphereComponent>(selectedEntity);
						}
					}

					ImGui::EndMenu();
				}
			}

			if (shouldShowWarning) {
				ImGui::OpenPopup("Warning");
			}

			if (ImGui::BeginPopupModal("Warning")) {
				ImGui::Text(warningText.c_str());

				if (ImGui::Button("Accept")) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::End();
		}
	}


	void displayEntityTreeView(uint32_t parent) {
		if (isEntityTreeViewVisible) {
			if (!ImGui::Begin("Entity Tree View", &isEntityTreeViewCollapsed)) {
				ImGui::End();
				return;
			}

			traverseEntityTree(*registry, *systems.transformSystem, parent);

			ImGui::End();
		}
	}

	void displayTime() {
		if (true) {
			if (!ImGui::Begin("##Time", nullptr)) {
				ImGui::End();
				return;
			}

			int ms = (int)((camera->deltaTime - (int)camera->deltaTime) * 1000);
			int s = (int)camera->deltaTime % 60;
			int m = (int)camera->deltaTime / 60;

			std::string time = std::to_string(m) + ":" + std::to_string(s) + "." + std::to_string(ms);

			ImGui::Text(time.c_str());

			if (ImGui::Button("Exit")) {
				camera->detach();
				close();
			}

			ImGui::End();
		}
	}

	void display() {
		displayEntityComponentView();
		displayMainMenu();
		displayMainMenuBar();
		displaySettings();
		displayRandomizer();
		displayEntityTreeView(0);
		displayTime();
	}
};

static bool isCursorEnabled = true;



Camera camera;

void mouseCallback(GLFWwindow* window, int button, int action, int modes) {
	if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS) {
		if (isCursorEnabled) {
			isCursorEnabled = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			isCursorEnabled = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		camera.jump();
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		camera.restart();
	}
}

int main() {
	if (!glfwInit()) {
		return 1;
	}

	int width = 1280;
	int height = 720;
	const char* title = "Game";
	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetKeyCallback(window, key_callback);

	if (!window) {
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (gl3wInit()) {
		return 1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsLight();
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	ImGui_ImplGLFW_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	Input input(window);

	/**/
	std::string vertexShaderSource0 =
		"precision mediump float;\n"
		"attribute vec3 a_Position;\n"
		"uniform mat4 u_Projection;\n"
		"uniform mat4 u_View;\n"
		"uniform mat4 u_Model;\n"
		"\n"
		"void main(void) {\n"
		"	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0f);\n"
		"}\n";

	std::string fragmentShaderSource0 =
		"precision mediump float;\n"
		"uniform vec4 u_Color;\n"
		"\n"
		"void main(void) {\n"
		"	gl_FragColor = u_Color;\n"
		"}\n";

	std::string vertexShaderSource1 =
		"precision mediump float;\n"
		"attribute vec3 a_Position;\n"
		"attribute vec4 a_Color;\n"
		"varying vec4 v_Color;\n"
		"uniform mat4 u_Projection;\n"
		"uniform mat4 u_View;\n"
		"uniform mat4 u_Model;\n"
		"\n"
		"void main(void) {\n"
		"	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0f);\n"
		"	v_Color = a_Color;\n"
		"}\n";

	std::string fragmentShaderSource1 =
		"precision mediump float;\n"
		"uniform vec4 u_Color;\n"
		"varying vec4 v_Color;\n"
		"\n"
		"void main(void) {\n"
		"	gl_FragColor = v_Color;\n"
		"}\n";

	std::string vertexShaderSource2 =
		"precision mediump float;\n"
		"attribute vec3 a_Position;\n"
		"attribute vec2 a_TexCoord;\n"
		"uniform mat4 u_Model;\n"
		"uniform mat4 u_View;\n"
		"uniform mat4 u_Projection;\n"
		"varying vec2 v_TexCoord;\n"
		"\n"
		"void main() {\n"
		"	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0f);\n"
		"	v_TexCoord = a_TexCoord;\n"
		"}\n";

	std::string fragmentShaderSource2 =
		"precision mediump float;\n"
		"uniform vec4 u_Color;\n"
		"varying vec2 v_TexCoord;\n"
		"uniform sampler2D u_Texture;\n"
		"\n"
		"void main() {\n"
		"	gl_FragColor =  texture(u_Texture, v_TexCoord) * u_Color;\n"
		"}\n";

	Program program(vertexShaderSource0, fragmentShaderSource0);
	Program program1(vertexShaderSource1, fragmentShaderSource1);
	Program program2(vertexShaderSource2, fragmentShaderSource2);

	entt::DefaultRegistry registry;

	Scene scene("", &systems);
	scene.setRegistry(&registry);

	systems.physics2DSystem = new Physics2DSystem(&registry);
	systems.physics3DSystem = new Physics3DSystem(&registry);
	systems.transformSystem = new TransformSystem(&registry, systems.physics3DSystem->getWorld());
	camera = Camera(&registry, &input, glm::vec3(0.0f), glm::vec3(0.0f), 30.0f);
	camera.setWorld3D(systems.physics3DSystem->getWorld());

	systems.pointSystem = new PointSystem(&registry, &camera, &program);
	systems.lineSystem = new LineSystem(&registry, &camera, &program);
	systems.shapeSystem = new ShapeSystem(&registry, &camera, &program1);
	systems.rectangleSystem = new RectangleSystem(&registry, &camera, &program2);
	systems.circleSystem = new CircleSystem(&registry, &camera, &program2);
	systems.cuboidSystem = new CuboidSystem(&registry, &camera, &program2);
	systems.sphereSystem = new SphereSystem(&registry, &camera, &program2);
	GUI gui(&registry, &camera, &scene, window);

	scene.root = registry.create();
	uint32_t& root = scene.root;
	registry.assign<TransformComponent>(root, "Root", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), "0");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	textures.insert(std::pair("Default", texture));

	TextureConf textureConf;
	textureConf.textureType = TextureType::MONOCHROMATIC;
	textureConf.width = 1;
	textureConf.height = 1;
	textureConf.color = glm::uvec3(255);
	unsigned char* data = generateTexture(textureConf);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	scene.textures = &textures;

	std::ifstream rTextureListFile;
	rTextureListFile.open("levels\\textureList.json");

	if (rTextureListFile.is_open()) {
		json textureListJSON;

		std::string texureListString((std::istreambuf_iterator<char>(rTextureListFile)), (std::istreambuf_iterator<char>()));
		textureListJSON = json::parse(texureListString);

		for (int a = 0; a < textureListJSON.size(); a++) {
			scene.loadTexture(textureListJSON[a]);
		}
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		input.update();

		systems.physics2DSystem->update();
		systems.physics3DSystem->update();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGLFW_NewFrame();
		ImGui::NewFrame();

		gui.display();

		glfwGetFramebufferSize(window, &width, &height);
		camera.setViewport(width, height);
		float x, y, z;
		processKeyboardInput(window, x, y, z);
		camera.update(glm::vec3(x, y, z));

		ImGui::Render();
		glViewport(0, 0, width, height);
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		systems.pointSystem->update();
		systems.lineSystem->update();
		systems.shapeSystem->update();
		systems.rectangleSystem->update();
		systems.circleSystem->update();
		systems.cuboidSystem->update();
		systems.sphereSystem->update();

		input.setLastPosition(input.getX(), input.getY());
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGLFW_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
