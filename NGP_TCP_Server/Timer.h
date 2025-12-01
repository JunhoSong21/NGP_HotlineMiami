#pragma once
#include <chrono>
#include <memory>
#include <mutex>

#include "GameEvent.h"
#include "EventQueue.h"

class Timer {
private:
	std::chrono::time_point<std::chrono::system_clock> moveEventCount;

	std::chrono::time_point<std::chrono::system_clock> timePoint;
	std::mutex timerMutex;

	void TimerLoop();

public:
	Timer();

};