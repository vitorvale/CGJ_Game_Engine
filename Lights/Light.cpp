#include "Light.h"

Light::Light()
{
	light.isEnabled = 1;
	light.isLocal = 1;
	float position[3] = { 0.0f, 0.0f, 0.0f };
	memcpy(light.position, position, 3 * sizeof(float));

	// only required for directional light
	float halfVector[3] = { 0.0f, 0.0f, 0.0f };
	memcpy(light.halfVector, halfVector, 3 * sizeof(float));

	// spotlight
	light.isSpot = 0;
	float coneDirection[3] = { 0.0f, 0.0f, 0.0f };
	memcpy(light.coneDirection, coneDirection, 3 * sizeof(float));
	light.spotCosCutoff = 0.0f;
	light.spotExponent = 0.0f;
}

LightProperties* Light::GetLightPtr()
{
	return &light;
}

void Light::ToggleLight() {
	light.isEnabled = light.isEnabled ? 0 : 1;
}

char* Light::GetLightType() {
	if (light.isLocal && light.isSpot)
	{
		return "SpotLight";
	}
	else if (!light.isLocal) 
	{
		return "DirectionalLight";
	}
	else
	{
		return "PointLight";
	}
}