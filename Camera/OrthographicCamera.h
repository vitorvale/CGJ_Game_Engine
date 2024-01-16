#include "Camera.h"
#pragma once
class OrthographicCamera : public Camera
{
private:
	float left;
	float right;
	float bottom;
	float top;
	float nearDistance;
	float farDistance;

public:
	OrthographicCamera(int width, int height, float look[3], float pos[3], float up[3], 
		float near, float far);
	OrthographicCamera() = default;
	void Update() override;
	void SetProjection() override;
};