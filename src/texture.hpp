#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include "noiseConf.hpp"
#include <string>

enum TextureType {
	MONOCHROMATIC, RANDOM, NOISE
};

class TextureConf {
public:
	std::string name = "";
	int width = 0;
	int height = 0;
	TextureType textureType = TextureType::MONOCHROMATIC;
	glm::uvec3 color = glm::uvec3(0.0f);
	NoiseConf noiseConf;
	glm::vec3 multiplier = glm::vec3(1.0f);
	const char* textureTypes[3] = { "MONOCHROMATIC", "RANDOM", "NOISE" };
};

unsigned char* generateTexture(TextureConf& textureConf);