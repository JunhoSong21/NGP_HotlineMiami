#include "pch.h"
#include "Timer.h"

Timer::Timer()
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTime);
}

Timer::~Timer()
{

}

float Timer::getDeltaTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime); // 현재 시간 가져오기

	float deltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;

	lastTime = currentTime;

	return deltaTime;
}
