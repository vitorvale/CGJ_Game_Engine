#include "Camera.h"

Camera::Camera(int width, int height, float look[3], float pos[3], float up[3])
{
	windowWidth = width;
	windowHeight = height;
	SetLook(look);
	SetPosition(pos);
	SetUp(up);
}

void Camera::SetLook(float newLook[3]) {
	std::copy(newLook, newLook + 3, lookPos);
}
float* Camera::GetLook() {
	return lookPos;
}

void Camera::SetPosition(float newPos[3]) {
	std::copy(newPos, newPos + 3, position);
}
float* Camera::GetPosition() {
	return position;
}

void Camera::SetUp(float newUp[3]) {
	std::copy(newUp, newUp + 3, upDir);
}
float* Camera::GetUp() {
	return upDir;
}

float* Camera::GetLookDirection() {
	float dir[3];
	subtract(lookPos, position, dir);
	normalize(dir);
	return dir; // FIXME returning pointer to local variable
}

float Camera::GetAspectRatio() {
	return (1.0f * windowWidth) / windowHeight;
}

void Camera::Update() {
	lookAt(position[0], position[1], position[2],
			lookPos[0], lookPos[1], lookPos[2],
			upDir[0], upDir[1], upDir[2]);
}

void Camera::ResizeWindow(int w, int h) {
	// Prevent a divide by zero, when window is too short
	if (h == 0)
		h = 1;

	windowWidth = w;
	windowHeight = h;

	// set the viewport to be the entire window
	glViewport(0, 0, windowWidth, windowHeight);

	SetProjection();
}

//void Camera::FollowTarget(float targetPos[3], float targetOffset[3], float deltaAlpha, float deltaBeta)
//{
//	// rotate
//	float* v = get(VIEW);
//	rotate(VIEW, deltaAlpha, v[4], v[5], v[6]);
//	rotate(VIEW, deltaBeta, v[0], v[1], v[2]);
//
//	//add(targetPos, targetOffset, position);
//	add(targetPos, targetOffset, position);
//	lookAt(position[0], position[1], position[2],
//		targetPos[0], targetPos[1], targetPos[2],
//		upDir[0], upDir[1], upDir[2]);
//}

void Camera::FollowTarget(float targetPos[3], float camPosition[3], float deltaAlpha, float deltaBeta)
{
	SetPosition(camPosition);
	// rotate
	float* v = get(VIEW);
	rotate(VIEW, deltaAlpha, v[4], v[5], v[6]);
	rotate(VIEW, deltaBeta, v[0], v[1], v[2]);

	//add(targetPos, targetOffset, position);
	lookAt(camPosition[0], camPosition[1], camPosition[2],
		targetPos[0], targetPos[1], targetPos[2],
		upDir[0], upDir[1], upDir[2]);
}
