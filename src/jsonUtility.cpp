#include "jsonUtility.hpp"

/* GLM to JSON and JSON to GLM conversions */
json glmToJSON(glm::vec3 value) {
	return { value.x, value.y, value.z };
}

json glmToJSON(glm::uvec3 value) {
	return { value.x, value.y, value.z };
}

json glmToJSON(glm::vec4 value) {
	return { value.x, value.y, value.z, value.w };
}

json glmToJSON(std::vector<glm::vec3> valueArray) {
	json valueArray1;

	for (int a = 0; a < valueArray.size(); a++) {
		json value = glmToJSON(valueArray[a]);
		valueArray1.push_back(value);
	}

	return valueArray1;
}

json glmToJSON(std::vector<glm::uvec3> valueArray) {
	json valueArray1;

	for (int a = 0; a < valueArray.size(); a++) {
		json value = glmToJSON(valueArray[a]);
		valueArray1.push_back(value);
	}

	return valueArray1;
}

json glmToJSON(std::vector<glm::vec4> valueArray) {
	json valueArray1;

	for (int a = 0; a < valueArray.size(); a++) {
		json value = glmToJSON(valueArray[a]);
		valueArray1.push_back(value);
	}
	return valueArray1;
}

glm::vec3 jsonToGLMVec3(json value) {
	return glm::vec3(value[0], value[1], value[2]);
}

glm::uvec3 jsonToGLMUvec3(json value) {
	return glm::uvec3(value[0], value[1], value[2]);
}

glm::vec4 jsonToGLMVec4(json value) {
	return glm::vec4(value[0], value[1], value[2], value[3]);
}

std::vector<glm::vec3> jsonToGLMVec3Vector(json valueArray) {
	std::vector<glm::vec3> valueArray1;

	for (int a = 0; a < valueArray.size(); a++) {
		json value = valueArray[a];
		valueArray1.push_back(jsonToGLMVec3(value));
	}

	return valueArray1;
}

std::vector<glm::uvec3> jsonToGLMUvec3Vector(json valueArray) {
	std::vector<glm::uvec3> valueArray1;

	for (int a = 0; a < valueArray.size(); a++) {
		json value = valueArray[a];
		valueArray1.push_back(jsonToGLMUvec3(value));
	}

	return valueArray1;
}

std::vector<glm::vec4> jsonToGLMVec4Vector(json valueArray) {
	std::vector<glm::vec4> valueArray1;

	for (int a = 0; a < valueArray.size(); a++) {
		json value = valueArray[a];
		valueArray1.push_back(jsonToGLMVec4(value));
	}

	return valueArray1;
}
/* GLM to JSON and JSON to GLM conversions */

/* PlayRho to JSON and JSON to PlayRho conversions */
json prToJSON(pr::Vec2 value) {
	return { value[0], value[1] };
}

pr::Vec2 jsonToPRVec2(json value) {
	return pr::Vec2(value[0], value[1]);
}

std::vector<pr::Vec2> jsonToPRVec2Vector(json valueArray) {
	std::vector<pr::Vec2> valueArray1;

	for (int a = 0; a < valueArray.size(); a++) {
		valueArray1.push_back(jsonToPRVec2(valueArray[a]));
	}

	return valueArray1;
}
/* PlayRho to JSON and JSON to PlayRho conversions */

/* Bullet to JSON and JSON to Bullet conversions */
json btToJSON(btVector3 value) {
	return { value.getX(), value.getY(), value.getZ() };
}

btVector3 jsonToBTVector3(json value) {
	return btVector3(value[0], value[1], value[2]);
}
/* Bullet to JSON and JSON to Bullet conversions */
