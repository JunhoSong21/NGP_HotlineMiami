#include "Timer.h"

Timer::Timer() :
	moveEventCount(0)
{
}

void Timer::TimerLoop()
{
	++moveEventCount;
	if (moveEventCount > 200) {
		
	}
}