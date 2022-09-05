#include "noiseConf.hpp"

const char* NoiseConf::noiseTypes[10] = {
	"Value",
	"Value Fractal",
	"Perlin",
	"Perlin Fractal",
	"Simplex",
	"Simplex Fractal",
	"Cellular",
	"White Noise",
	"Cubic",
	"Cubiic Fractal"
};

const char* NoiseConf::fractalTypes[3] = {
	"FBM",
	"Billow",
	"Rigid Multi"
};

const char* NoiseConf::cellularReturnTypes[8] = {
	"Cell Value",
	"Noise Lookup",
	"Distance",
	"Distance 2",
	"Distance 2 Add",
	"Distance 2 Sub",
	"Distance 2 Mul",
	"Distance 2 Div"
};

const char* NoiseConf::cellularDistanceFunctions[3] = {
	"Euiclidean",
	"Manhattan",
	"Natural"
};

const char* NoiseConf::interps[3]{
	"Linear",
	"Hermite",
	"Quintic"
};

NoiseConf::NoiseConf() {

}

NoiseConf::~NoiseConf() {}
