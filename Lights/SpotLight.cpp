#include "SpotLight.h"

SpotLight::SpotLight(float position[4], float coneDirection[4]) : Light()
{
	memcpy(light.position, position, 4 * sizeof(float));

	light.isSpot = 1;
	memcpy(light.coneDirection, coneDirection, 4 * sizeof(float));
	light.spotCosCutoff = 0.9f; 
	light.spotExponent = 0.1f; 
}

void SpotLight::SetPosition(float newPosition[4]) {
	memcpy(light.position, newPosition, 4 * sizeof(float));
}
void SpotLight::SetConeDirection(float newDirection[4]) {
	memcpy(light.coneDirection, newDirection, 4 * sizeof(float));
}

void SpotLight::UpdateTransform(MatrixTypes m, float translate[4]) {
	//printf("updatetransform: %f %f %f %f\n", light.position[0], light.position[1], light.position[2], light.position[3]);
	multMatrixPoint(m, translate, light.position);
	//printf("updatetransform: %f %f %f %f\n", light.position[0], light.position[1], light.position[2], light.position[3]);
}
