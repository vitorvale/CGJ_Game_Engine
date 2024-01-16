#include "PointLight.h"

PointLight::PointLight(float position[4]) : Light()
{
	memcpy(light.position, position, 4 * sizeof(float));
}
