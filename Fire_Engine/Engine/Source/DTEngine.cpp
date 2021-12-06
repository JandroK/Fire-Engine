#include "DTEngine.h"

int DTEngine::frameCount;

float DTEngine::time;
float DTEngine::timeScale = 1.f;
float DTEngine::deltaTime;
float DTEngine::realStartTime;
float DTEngine::realTimeDT;
float DTEngine::realTimeFromStartUp;

Timer DTEngine::gameTimer;
DTGState DTEngine::state = DTGState::STOP;

void DTEngine::PlayGame()
{
	state = DTGState::PLAY;
	gameTimer.Start();

	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void DTEngine::StopGame()
{
	state = DTGState::STOP;
	time = 0.f;
	frameCount = 0;
	deltaTime = 0.f;

	gameTimer.Stop();
	SDL_SetRelativeMouseMode(SDL_FALSE);
}

void DTEngine::PreUpdate()
{

	if (state == DTGState::PLAY || state == DTGState::STEP) {
		frameCount++;
		time += realTimeDT;
		deltaTime = realTimeDT * timeScale;
	}

	switch (state)
	{
	case DTGState::PAUSE:
		deltaTime = 0.f;
		break;
	case DTGState::STOP:
		time = 0.0f;
		break;
	case DTGState::PRESTEP:
		state = DTGState::STEP;
		break;
	case DTGState::STEP:
		time += realTimeDT;
		deltaTime = realTimeDT * timeScale;
		state = DTGState::PAUSE;
		break;
	}
}



const char* DTEngine::GetStateString()
{
	char* ret = "";
	switch (state)
	{
	case DTGState::PLAY:
		ret = "Play";
		break;
	case DTGState::PAUSE:
		ret = "Pause";
		break;
	case DTGState::STOP:
		ret = "Stop";
		break;
	case DTGState::STEP:
		ret = "Step";
		break;
	default:
		break;
	}

	return ret;
}

void DTEngine::PauseGame()
{
	if (state == DTGState::PAUSE)
	{
		ResumeGame();
		return;
	}

	if (state == DTGState::PLAY || state == DTGState::STEP)
	{
		state = DTGState::PAUSE;
		gameTimer.Stop();
	}
}

void DTEngine::StepGame()
{
	if (state == DTGState::PAUSE)
		state = DTGState::PRESTEP;
}

void DTEngine::ResumeGame()
{
	if (state != DTGState::STEP)
		state = DTGState::PLAY;

	gameTimer.Start();
}
