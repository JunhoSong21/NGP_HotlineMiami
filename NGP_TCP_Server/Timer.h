#pragma once
#include <chrono>

class Timer {
private:
	std::chrono::milliseconds interval;

	void TimerLoop();

public:
	Timer();

};