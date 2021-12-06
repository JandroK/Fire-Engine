#pragma once
#include"Timer.h"

enum class DTGState {
	PLAY,
	PAUSE,
	STOP,
	STEP,
	PRESTEP,
};

struct DTEngine
{
public:

	static void PlayGame();
	static void PauseGame();
	static void ResumeGame();
	static void StopGame();
	static void StepGame();

	static void PreUpdate();
	static Timer gameTimer;

	static const char* GetStateString();
	
	//Seconds since game start
	static float time;

	//Scale of time 0.5 = 2 times slower
	static float timeScale;

	//Last frame completion time
	static float deltaTime;

	//Frames since game to start
	static int frameCount;

	//Real time counters
	static float realTimeFromStartUp;
	static float realTimeDT;
	static float realStartTime;

	static DTGState state;

};