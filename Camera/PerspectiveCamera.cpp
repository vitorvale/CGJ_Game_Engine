#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(float width, float height, float look[3], float pos[3], float up[3],
	float sfov, float near, float far) : Camera(width, height, look, pos, up)
{
	fov = sfov;
	nearDistance = near;
	farDistance = far;
}

void PerspectiveCamera::Update()
{
	Camera::Update();
}

void PerspectiveCamera::SetProjection()
{
	loadIdentity(PROJECTION);
	perspective(fov, GetAspectRatio(), nearDistance, farDistance);
}
