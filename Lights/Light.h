#pragma once
#include <cstring>
#include "../AVTmathLib.h"

struct LightProperties {
	int isEnabled;
	int isLocal;
	int isSpot;
	float position[4];
	float halfVector[3];
	float coneDirection[4];
	float spotCosCutoff;
	float spotExponent;
};

class Light {
public:
	Light();

	void ToggleLight();
	char* GetLightType();
	
	LightProperties* GetLightPtr();
	LightProperties light;
};