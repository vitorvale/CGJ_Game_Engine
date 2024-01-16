#pragma once
#include <GL/glew.h>
#include <AVTmathLib.h>
#include "TimeUtil.h"
#include "vsShaderLib.h"
#include "avtFreeType.h"
#include <string>

class GameHudManager
{
private:
	VSShaderLib* textShader;
	int points;
	int lives;
	bool isPaused;

public:
	GameHudManager(VSShaderLib* txtShader);
	GameHudManager() = default;
	void TogglePause();
	void RestartGame();
	void IncrementPoints(int pointsToAdd);
	void DecreaseLives();
	bool UpdateHUD();
	void RenderPointsText(int viewport[4]);
	void RenderLivesText(int viewport[4]);
	void RenderPauseText(int viewport[4]);
	void RenderGameOverText(int viewport[4]);
	int GetLives() { return lives; }
};