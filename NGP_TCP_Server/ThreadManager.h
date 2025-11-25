#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>

#include "NetworkThread.h"
#include "DataManager.h"
#include "Player.h"

class ThreadManager {
private:
	std::unordered_map<int, std::unique_ptr<NetworkThread>> networkThreads;

	mutable std::mutex threadMutex;
	std::atomic<int> threadId;

	ThreadManager();
	ThreadManager(const ThreadManager&) = delete;
	ThreadManager& operator=(const ThreadManager&) = delete;

public:
	static ThreadManager& GetInstance()
	{
		static ThreadManager instance;
		return instance;
	}

	void AddThread(SOCKET clientSock);
	void RemoveThread(int id);
	int ThreadCount();

	void BroadcastEvent(std::unique_ptr<GameEvent> event);
};