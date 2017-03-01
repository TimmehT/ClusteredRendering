#include <DirectXPCH.h>
#include "GameTimer.h"

GameTimer::GameTimer() : m_secondsPerCount(0.0), m_deltaTime(-1.0), m_baseTime(0),
m_pausedTime(0), m_prevTime(0), m_currTime(0), m_stopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimer::TotalTime()const
{
	if (m_stopped)
	{
		return (float)(((m_stopTime - m_pausedTime) - m_baseTime)*m_secondsPerCount);
	}
	else
	{
		return (float)(((m_currTime - m_pausedTime) - m_baseTime)*m_secondsPerCount);
	}
}

float GameTimer::DeltaTime()const
{
	return (float)m_deltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_baseTime = currTime;
	m_prevTime = currTime;
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
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_stopTime = currTime;
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

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	// Calc time diff between current frame and previous
	m_deltaTime = (m_currTime - m_prevTime)*m_secondsPerCount;

	// Set m_prevTime to m_currTime in prep for next frame
	m_prevTime = m_currTime;

	/* Force m_deltaTime to a non -ive value incase of pause
	forces to another processor core/thread which can negate delta time*/
	if (m_deltaTime < 0.0)
	{
		m_deltaTime = 0.0;
	}
}