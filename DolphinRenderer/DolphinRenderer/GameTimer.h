#pragma once
#ifndef GAME_TIMER_H
#define GAME_TIMER_H

#include "Headers.h"

class GameTimer {
public:
	GameTimer();

public:
	float GameTime() const;
	float DeltaTime() const { return static_cast<float>(m_deltaTime); }

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_prevTime;
	__int64 m_currTime;

	bool m_stopped;
};

#endif /* !GAME_TIMER_H */
