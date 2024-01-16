#include "TimeUtil.h"

float TimeUtil::deltaTime = 0.0f;

double TimeUtil::previousFrameElapsedTime = 0.0f;

double TimeUtil::ElapsedTime_s()
{
	return glutGet(GLUT_ELAPSED_TIME) * 0.001f;
}

double TimeUtil::ElapsedTime_ms()
{
	return glutGet(GLUT_ELAPSED_TIME);
}

void TimeUtil::UpdateDeltaTime()
{
	double timeSinceStart = ElapsedTime_s();
	deltaTime = (float)(timeSinceStart - previousFrameElapsedTime);
	previousFrameElapsedTime = timeSinceStart;
}