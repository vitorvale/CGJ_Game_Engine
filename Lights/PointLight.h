#pragma once
#include "Light.h"
class PointLight : public Light {
public:
	PointLight(float position[4]);
	PointLight() = default;
};