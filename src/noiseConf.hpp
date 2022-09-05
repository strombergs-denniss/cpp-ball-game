#pragma once
#include "FastNoise.h"

class NoiseConf : public FastNoise {
public:
	friend class GUI;
	static const char* noiseTypes[10];
	static const char* fractalTypes[3];
	static const char* cellularReturnTypes[8];
	static const char* cellularDistanceFunctions[3];
	static const char* interps[3];
	float amplitude = 255.0f;
public:
	NoiseConf();
	~NoiseConf();
};
