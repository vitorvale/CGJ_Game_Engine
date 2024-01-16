#pragma once
#include "Light.h"

class SpotLight : public Light {
public:
	SpotLight(float position[4], float coneDirection[4]);
	SpotLight() = default;

	void SetPosition(float newPosition[4]);
	void SetConeDirection(float newDirection[4]);
	void UpdateTransform(MatrixTypes m, float translate[4]);
};