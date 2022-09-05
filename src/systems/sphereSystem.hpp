#pragma once
#include "rendererSystem.hpp"
#include "../components/transformComponent.hpp"
#include "../components/sphereComponent.hpp"

class Vec3KeyFuncs {
public:
	size_t operator()(const glm::vec3& k)const;
	bool operator()(const glm::vec3& a, const glm::vec3& b)const;
};

class SphereSystem : public RendererSystem {
private:
	Program* program;
	GLuint positionBuffer;
	GLuint elementArrayBuffer;
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices;
	Camera* camera;
	GLuint texture;
	GLuint texCoordBuffer;
public:
	const float GOLDEN_RATIO = 1.61803398875f;
	const int ICOSOHEDRON_VERTEX_COUNT = 12;
	const glm::vec3 ICOSOHEDRON_VERTICES[12] = {
		glm::vec3(-1.0f, GOLDEN_RATIO, 0.0f),
		glm::vec3(1.0f, GOLDEN_RATIO, 0.0f),
		glm::vec3(-1.0f, -GOLDEN_RATIO, 0.0f),
		glm::vec3(1.0f, -GOLDEN_RATIO, 0.0f),

		glm::vec3(0.0f, -1.0f, GOLDEN_RATIO),
		glm::vec3(0.0f, 1.0f, GOLDEN_RATIO),
		glm::vec3(0.0f, -1.0, -GOLDEN_RATIO),
		glm::vec3(0.0f, 1.0f, -GOLDEN_RATIO),

		glm::vec3(GOLDEN_RATIO, 0.0f, -1.0f),
		glm::vec3(GOLDEN_RATIO, 0.0f, 1.0f),
		glm::vec3(-GOLDEN_RATIO, 0.0f, -1.0f),
		glm::vec3(-GOLDEN_RATIO, 0.0, 1.0f)
	};

	const int ICOSOHEDRON_INDEX_COUNT = 60;
	const uint32_t ICOSOHEDRON_INDICES[60] = {
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,

		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,

		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,

		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};
	std::vector<glm::vec2> uvs;
private:
	void prepareToRenderSpheres();
public:
	SphereSystem(entt::DefaultRegistry* registry, Camera* camera, Program* program);
	void update() override;
	glm::vec3 findMidpoint(glm::vec3 vertex1, glm::vec3 vertex2);
	void generateIcosphereMesh(size_t lod, std::vector<uint32_t>& indices, std::vector<glm::vec3>& positions, std::vector<glm::vec2>& uvs);
};
