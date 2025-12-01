#pragma once
#include <chrono>
#include <memory>
#include <mutex>

#include "GameEvent.h"
#include "EventQueue.h"

class Timer {
private:
	std::chrono::time_point<std::chrono::system_clock> moveEventPoint;

	std::chrono::time_point<std::chrono::system_clock> timePoint;
	std::mutex timerMutex;

public:
	Timer();

	static Timer& GetInstance()
	{
		static Timer instance;
		return instance;
	}

	void TimerLoop();
};