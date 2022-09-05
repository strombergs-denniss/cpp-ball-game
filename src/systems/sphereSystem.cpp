#include "sphereSystem.hpp"

size_t Vec3KeyFuncs::operator()(const glm::vec3& k)const {
	return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
}

bool Vec3KeyFuncs::operator()(const glm::vec3& a, const glm::vec3& b)const {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

glm::vec3 SphereSystem::findMidpoint(glm::vec3 vertex1, glm::vec3 vertex2) {
	return glm::normalize(glm::vec3((vertex1.x + vertex2.x) / 2.0f, (vertex1.y + vertex2.y) / 2.0f, (vertex1.z + vertex2.z) / 2.0f));
}

void SphereSystem::generateIcosphereMesh(size_t lod, std::vector<uint32_t> & indices, std::vector<glm::vec3> & positions, std::vector<glm::vec2>& uvs) {
	std::vector<uint32_t> newIndices;
	newIndices.reserve(256);

	std::unordered_map<glm::vec3, uint32_t, Vec3KeyFuncs, Vec3KeyFuncs> vertexLookup;

	indices.resize(ICOSOHEDRON_INDEX_COUNT);

	for (uint32_t i = 0; i < ICOSOHEDRON_INDEX_COUNT; i++) {
		indices[i] = ICOSOHEDRON_INDICES[i];
	}

	positions.resize(ICOSOHEDRON_VERTEX_COUNT);

	for (uint32_t i = 0; i < ICOSOHEDRON_VERTEX_COUNT; i++) {
		positions[i] = glm::normalize(ICOSOHEDRON_VERTICES[i]);
		vertexLookup[glm::normalize(ICOSOHEDRON_VERTICES[i])] = i;
	}

	for (size_t i = 0; i < (size_t)lod; i++) {
		for (size_t j = 0; j < indices.size(); j += 3) {
			glm::vec3 vertex1 = positions[indices[j + 0]];
			glm::vec3 vertex2 = positions[indices[j + 1]];
			glm::vec3 vertex3 = positions[indices[j + 2]];

			glm::vec3 midPoint12 = findMidpoint(vertex1, vertex2);
			glm::vec3 midPoint23 = findMidpoint(vertex2, vertex3);
			glm::vec3 midPoint13 = findMidpoint(vertex1, vertex3);

			uint32_t mp12Index;
			uint32_t mp23Index;
			uint32_t mp13Index;

			auto iter = vertexLookup.find(midPoint12);
			if (iter != vertexLookup.end()) {
				mp12Index = iter->second;
			}
			else {
				mp12Index = (uint32_t)positions.size();
				positions.push_back(midPoint12);
				vertexLookup[midPoint12] = mp12Index;
			}

			iter = vertexLookup.find(midPoint23);

			if (iter != vertexLookup.end()) {
				mp23Index = iter->second;
			}
			else {
				mp23Index = (uint32_t)positions.size();
				positions.push_back(midPoint23);
				vertexLookup[midPoint23] = mp23Index;
			}

			iter = vertexLookup.find(midPoint13);

			if (iter != vertexLookup.end()) {
				mp13Index = iter->second;
			}
			else {
				mp13Index = (uint32_t)positions.size();
				positions.push_back(midPoint13);
				vertexLookup[midPoint13] = mp13Index;
			}

			newIndices.push_back(indices[j]);
			newIndices.push_back(mp12Index);
			newIndices.push_back(mp13Index);
			newIndices.push_back(mp12Index);
			newIndices.push_back(indices[j + 1]);
			newIndices.push_back(mp23Index);
			newIndices.push_back(mp13Index);
			newIndices.push_back(mp23Index);
			newIndices.push_back(indices[j + 2]);
			newIndices.push_back(mp12Index);
			newIndices.push_back(mp23Index);
			newIndices.push_back(mp13Index);
		}

		indices.swap(newIndices);
		newIndices.clear();
	}

	for (int a = 0; a < vertices.size(); a++) {
		glm::vec3 n = glm::normalize(vertices[a]);
		float u = glm::atan(n.x, glm::abs(n.z)) / (2 * glm::pi<float>()) + 0.5f;
		float v = n.y * 0.5f + 0.5f;

		uvs.push_back(glm::vec2(u, v));
	}
}

void SphereSystem::prepareToRenderSpheres() {
	program->use();

	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
	program->setAttrib("a_Position", 3, GL_FLOAT, GL_FALSE, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
	program->setAttrib("a_TexCoord", 2, GL_FLOAT, GL_FALSE, 0);

	program->setUniformMat4("u_Projection", camera->getProjection());
	program->setUniformMat4("u_View", camera->getView());
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
}


SphereSystem::SphereSystem(entt::DefaultRegistry * registry, Camera * camera, Program * program) : RendererSystem(registry) {
	this->camera = camera;
	this->program = program;

	generateIcosphereMesh(2, indices, vertices, uvs);

	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &elementArrayBuffer);
	glGenBuffers(1, &texCoordBuffer);

	program->setAttrib("a_TexCoord", 3, GL_FLOAT, GL_FALSE, 0);
}

void SphereSystem::update() {
	prepareToRenderSpheres();

	auto view = registry->view<TransformComponent, SphereComponent>();

	for (auto entity : view) {
		auto& transformComp = view.get<TransformComponent>(entity);
		auto& sphereComp = view.get<SphereComponent>(entity);

		program->setUniformMat4("u_Model", transformComp.getModel());
		program->setUniformVec4("u_Color", sphereComp.getColor());
		glBindTexture(GL_TEXTURE_2D, sphereComp.getTexture());
		program->setUniform1i("u_Texture", 0);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	}
}
