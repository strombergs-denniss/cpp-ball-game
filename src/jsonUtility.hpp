#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <PlayRho/PlayRho.hpp>
#include <Bullet/btBulletDynamicsCommon.h>
#include <Bullet/btBulletCollisionCommon.h>

using json = nlohmann::json;
namespace pr = playrho;
namespace pr2D = pr::d2;

/* GLM to JSON and JSON to GLM conversions */
json glmToJSON(glm::vec3 value);

json glmToJSON(glm::uvec3 value);

json glmToJSON(glm::vec4 value);

json glmToJSON(std::vector<glm::vec3> valueArray);

json glmToJSON(std::vector<glm::uvec3> valueArray);

json glmToJSON(std::vector<glm::vec4> valueArray);

glm::vec3 jsonToGLMVec3(json value);

glm::uvec3 jsonToGLMUvec3(json value);

glm::vec4 jsonToGLMVec4(json value);

std::vector<glm::vec3> jsonToGLMVec3Vector(json valueArray);

std::vector<glm::uvec3> jsonToGLMUvec3Vector(json valueArray);

std::vector<glm::vec4> jsonToGLMVec4Vector(json valueArray);
/* GLM to JSON and JSON to GLM conversions */

/* PlayRho to JSON and JSON to PlayRho conversions */
json prToJSON(pr::Vec2 value);

pr::Vec2 jsonToPRVec2(json value);

std::vector<pr::Vec2> jsonToPRVec2Vector(json valueArray);
/* PlayRho to JSON and JSON to PlayRho conversions */

/* Bullet to JSON and JSON to Bullet conversions */
json btToJSON(btVector3 value);

btVector3 jsonToBTVector3(json value);
/* Bullet to JSON and JSON to Bullet conversions */
