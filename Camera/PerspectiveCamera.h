#include "Camera.h"
#pragma once
class PerspectiveCamera : public Camera
{
private:
	float fov;
	float nearDistance;
	float farDistance;

public:
	PerspectiveCamera(float width, float height, float look[3], float pos[3], float up[3],
		float fov, float near, float far);
	PerspectiveCamera() = default;
	void Update() override;
	void SetProjection() override;
};