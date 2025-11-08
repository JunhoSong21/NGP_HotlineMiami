#pragma once
#include <concurrent_queue.h>
#include <memory>

#include "GameEvent.h"

using namespace Concurrency;

class EventManager {
private:
	concurrent_queue<std::unique_ptr<GameEvent>> EventQueue;

	EventManager() = default;
	EventManager(const EventManager&) = delete;
	EventManager& operator=(const EventManager&) = delete;

public:
	static EventManager& GetInstance()
	{
		static EventManager instance;
		return instance;
	}

	void PushEvent(std::unique_ptr<GameEvent> event);
	bool PopEvent(std::unique_ptr<GameEvent>& event);
	bool IsInEvent();
};

