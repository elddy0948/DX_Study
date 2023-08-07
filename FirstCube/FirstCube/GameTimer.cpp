#include "GameTimer.h"

GameTimer::GameTimer()
	: mSecondsPerCount(0.0), mDeltaTime(0.0),
	mBaseTime(0), mPausedTime(0), mStopTime(0),
	mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimer::DeltaTime() const
{
	return (float)mDeltaTime;
}

float GameTimer::GameTime() const
{
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
}

void GameTimer::Reset()
{
	__int64 currentTime;
	QueryPerformanceFrequency((LARGE_INTEGER*)&currentTime);

	mBaseTime = currentTime;
	mPrevTime = currentTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		__int64 currentTime;
		QueryPerformanceFrequency((LARGE_INTEGER*)&currentTime);

		mStopTime = currentTime;
		mStopped = true;
	}
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceFrequency((LARGE_INTEGER*)&startTime);

	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	// get this frame time
	__int64 currentTime;
	QueryPerformanceFrequency((LARGE_INTEGER*)&currentTime);
	mCurrTime = currentTime;

	// get delta time 
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	// ready for next frame
	mPrevTime = mCurrTime;

	if (mDeltaTime < 0.0) { mDeltaTime = 0.0; }
}