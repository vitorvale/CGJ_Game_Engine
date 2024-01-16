#include "../AVTmathLib.h"
#pragma once
class Camera
{
private:
	float lookPos[3];
	float position[3];
	float upDir[3];


public:
	int windowWidth;
	int windowHeight;

	Camera(int width, int height, float look[3], float pos[3], float up[3]);
	Camera() = default;	
	
	void SetLook(float newLook[3]);
	float* GetLook();

	void SetPosition(float newDir[3]);
	float* GetPosition();

	void SetUp(float newDir[3]);
	float* GetUp();

	float* GetLookDirection();

	float GetAspectRatio();

	virtual void Update() = 0;
	
	void ResizeWindow(int w, int h);
	void FollowTarget(float targetPos[3], float camPosition[3], float deltaAlpha, float deltaBeta);
	virtual void SetProjection() = 0;

};