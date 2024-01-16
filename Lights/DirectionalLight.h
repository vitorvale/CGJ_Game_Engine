#pragma once
#include "Light.h"
#include "../AVTmathLib.h"
class DirectionalLight : public Light {
private:
	bool isDay = true;
	
public:
	DirectionalLight();
};