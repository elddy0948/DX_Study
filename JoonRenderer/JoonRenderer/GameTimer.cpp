#include "GameTimer.h"

GameTimer::GameTimer()
	: m_secondsPerCount(0.0), m_deltaTime(0.0), m_baseTime(0),
	m_pausedTime(0), m_stopTime(0), m_prevTime(0), m_currentTime(0),
	m_stopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimer::GameTime() const
{
	if (m_stopped)
	{
		return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}
	else
	{
		return (float)(((m_currentTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}
}

float GameTimer::DeltaTime() const
{
	return (float)m_deltaTime;
}

void GameTimer::Reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	
	m_baseTime = currentTime;
	m_prevTime = currentTime;
	m_stopTime = 0;
	m_stopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (m_stopped)
	{
		m_pausedTime += (startTime - m_stopTime);
		m_prevTime = startTime;
		m_stopTime = 0;
		m_stopped = false;
	}
}

void GameTimer::Stop()
{
	if (!m_stopped)
	{
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		m_stopTime = currentTime;
		m_stopped = true;
	}
}

void GameTimer::Tick()
{
	if (m_stopped)
	{
		m_deltaTime = 0.0;
		return;
	}

	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	m_currentTime = currentTime;

	m_deltaTime = (m_currentTime - m_prevTime) * m_secondsPerCount;

	m_prevTime = m_currentTime;

	if (m_deltaTime < 0.0) { m_deltaTime = 0.0; }
}