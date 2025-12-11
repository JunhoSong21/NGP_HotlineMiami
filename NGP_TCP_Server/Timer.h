#pragma once
#include <chrono>
#include <memory>
#include <mutex>
#include <array>
#include <algorithm>

#include "GameEvent.h"
#include "EventQueue.h"
#include "DataManager.h"
#include "GlobalData.h"

class Timer {
private:
	std::chrono::time_point<std::chrono::system_clock> moveEventPoint;
	std::chrono::time_point<std::chrono::system_clock> bulletEventPoint;
	std::chrono::time_point<std::chrono::system_clock> gameEndPoint;

	std::chrono::time_point<std::chrono::system_clock> timePoint;
	std::mutex timerMutex;

	std::array<std::chrono::time_point<std::chrono::system_clock>, MAX_CLIENT_NUM> bulletArray;
	std::array<bool, MAX_CLIENT_NUM> isBulletExist;

	std::array<std::chrono::time_point<std::chrono::system_clock>, MAX_CLIENT_NUM> grenadeArray;
	std::array<bool, MAX_CLIENT_NUM> isGrenadeExist;
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

	void SetBulletArray(int i);

	bool GetBulletArray(int i);
	bool GetGrenadeArray(int i);
};