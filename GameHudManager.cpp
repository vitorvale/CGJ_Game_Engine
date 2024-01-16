#include "GameHudManager.h"

GameHudManager::GameHudManager(VSShaderLib* txtShader) {
	lives = 5;
	points = 0;
	textShader = txtShader;
	isPaused = false;
}
void GameHudManager::TogglePause() {
	isPaused = !isPaused;
}
void GameHudManager::RestartGame() {
	lives = 5;
	points = 0;
	// reset car
	// reset oranges
	//reset TimeUtils::elapsedtime
}

void GameHudManager::IncrementPoints(int pointsToAdd) {
	points += pointsToAdd;
}

void GameHudManager::DecreaseLives() {
	if(lives>0)
		lives--;
}

bool GameHudManager::UpdateHUD() {
	//Render text (bitmap fonts)
	glDisable(GL_DEPTH_TEST);
	//the glyph contains background colors and non-transparent for the actual character pixels. So we use the blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	//viewer looking down at negative z direction
	pushMatrix(MODEL);
	loadIdentity(MODEL);
	pushMatrix(PROJECTION);
	loadIdentity(PROJECTION);
	pushMatrix(VIEW);
	loadIdentity(VIEW);
	ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
	
	//render text here
	RenderPointsText(m_viewport);
	RenderLivesText(m_viewport);
	if (isPaused && lives != 0)
	{
		RenderPauseText(m_viewport);
	}
	if (lives == 0)
	{
		// pause game
		RenderGameOverText(m_viewport);
	}

	popMatrix(PROJECTION);
	popMatrix(VIEW);
	popMatrix(MODEL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	if (lives == 0)
		return false;
	else
		return true;
}

void GameHudManager::RenderPointsText(int viewport[4])
{
	float posX = (viewport[0] + viewport[2] - 1) * 0.4f;
	float posY = (viewport[1] + viewport[3] - 1) * 0.9f;
	std::string t = "Points: " + std::to_string(points);
	RenderText(*textShader, t, posX, posY, 0.5f, 0.0f, 0.0f, 0.0f);
}
void GameHudManager::RenderLivesText(int viewport[4])
{
	float posX = (viewport[0] + viewport[2] - 1) * 0.05f;
	float posY = (viewport[1] + viewport[3] - 1) * 0.9f;
	std::string t = "Lives: " + std::to_string(lives);
	RenderText(*textShader, t, posX, posY, 0.5f, 0.0f, 0.0f, 0.0f);
}
void GameHudManager::RenderPauseText(int viewport[4])
{
	float posX = (viewport[0] + viewport[2] - 1) * 0.3f;
	float posY = (viewport[1] + viewport[3] - 1) * 0.5f;
	RenderText(*textShader, "Game paused", posX, posY, 0.5f, 0.0f, 0.0f, 0.0f);
	posX = (viewport[0] + viewport[2] - 1) * 0.25f;
	posY = (viewport[1] + viewport[3] - 1) * 0.4f;
	RenderText(*textShader, "Press \"S\" to continue", posX, posY, 0.3f, 0.0f, 0.0f, 0.0f);
}

void GameHudManager::RenderGameOverText(int viewport[4])
{
	float posX = (viewport[0] + viewport[2] - 1) * 0.3f;
	float posY = (viewport[1] + viewport[3] - 1) * 0.5f;
	RenderText(*textShader, "GAME OVER", posX, posY, 0.5f, 0.0f, 0.0f, 0.0f);
	posX = (viewport[0] + viewport[2] - 1) * 0.25f;
	posY = (viewport[1] + viewport[3] - 1) * 0.4f;
	RenderText(*textShader, "Press \"R\" to continue", posX, posY, 0.3f, 0.0f, 0.0f, 0.0f);
}

