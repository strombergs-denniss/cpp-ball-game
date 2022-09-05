#include "texture.hpp"

unsigned char* generateTexture(TextureConf& textureConf) {
	unsigned char* data;

	if (textureConf.textureType == TextureType::RANDOM) {
		data = new unsigned char[textureConf.width * textureConf.height * 3];

		for (int a = 0; a < textureConf.width * textureConf.height * 3; a += 3) {
			data[a] = glm::linearRand(0, 255);
			data[a + 1] = glm::linearRand(0, 255);
			data[a + 2] = glm::linearRand(0, 255);
		}
	} else if (textureConf.textureType == TextureType::MONOCHROMATIC) {
		data = new unsigned char[3];

		data[0] = (unsigned char)textureConf.color.x;
		data[1] = (unsigned char)textureConf.color.y;
		data[2] = (unsigned char)textureConf.color.z;
		textureConf.width = 1;
		textureConf.height = 1;
	}
	else {
		data = new unsigned char[textureConf.width * textureConf.height * 3];

		for (int x = 0; x < textureConf.width; x++) {
			for (int y = 0; y < textureConf.height; y++) {
				int xy = x * textureConf.width + y;

				char v = textureConf.noiseConf.GetNoise(x, y) * textureConf.noiseConf.amplitude;

				data[xy * 3] = v * textureConf.multiplier.x;
				data[xy * 3 + 1] = v * textureConf.multiplier.y;
				data[xy * 3 + 2] = v * textureConf.multiplier.z;
			}
		}
	}

	return data;
}
