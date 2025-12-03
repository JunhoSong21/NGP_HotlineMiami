#pragma once
#include <chrono>
#include <memory>
#include <mutex>
#include <array>

#include "GameEvent.h"
#include "EventQueue.h"

class Timer {
private:
	std::chrono::time_point<std::chrono::system_clock> moveEventPoint;
	std::chrono::time_point<std::chrono::system_clock> bulletEventPoint;

	std::chrono::time_point<std::chrono::system_clock> timePoint;
	std::mutex timerMutex;

	std::array<std::chrono::time_point<std::chrono::system_clock>, 3> bulletArray;
	std::array<bool, 3> isBulletExist;

	std::array<std::chrono::time_point<std::chrono::system_clock>, 3> grenadeArray;
	std::array<bool, 3> isGrenadeExist;
public:
	Timer();

	static Timer& GetInstance()
	{
		static Timer instance;
		return instance;
	}

	void TimerLoop();

	bool AddBullet(int id);
	bool AddGrenade(int id);
};