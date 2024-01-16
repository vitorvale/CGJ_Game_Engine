#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light()
{
	light.isLocal = 0;
	float direction[4] = { 0.0f, -1.0f, 0.0f, 0.0f };
	normalize(direction);
	memcpy(light.position, direction, 4 * sizeof(float)); // in this case position represents direction
	float halfVector[3] = { 0.0f, 1.0f, 0.0f };
	memcpy(light.halfVector, halfVector, 3 * sizeof(float));
}
