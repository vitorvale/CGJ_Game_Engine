#pragma once
#include <GL/freeglut.h>

class TimeUtil {
public:
	static float deltaTime;
	static double previousFrameElapsedTime;
	static double ElapsedTime_s();
	static double ElapsedTime_ms();
	static void UpdateDeltaTime();
};