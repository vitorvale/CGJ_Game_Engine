#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(int width, int height, float look[3], float pos[3], float up[3]
	, float near, float far) : Camera(width, height, look, pos, up)
{
	left = -100;
	right = 100;
	bottom = -100;
	top = 100;
	nearDistance = near;
	farDistance = far;
}

void OrthographicCamera::Update()
{
	Camera::Update();
}

void OrthographicCamera::SetProjection()
{
	loadIdentity(PROJECTION);
	ortho(left, right, bottom, top, nearDistance, farDistance);
}