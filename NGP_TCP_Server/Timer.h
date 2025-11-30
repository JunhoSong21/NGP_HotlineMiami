#pragma once
#include <chrono>
#include <atomic>
#include <memory>

#include "EventQueue.h"

class Timer {
private:
	std::atomic<int> moveEventCount;

	void TimerLoop();

public:
	Timer();

};