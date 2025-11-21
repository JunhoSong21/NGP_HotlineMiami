#pragma once
#include <concurrent_queue.h>
#include <memory>

#include "GameEvent.h"

using namespace Concurrency;

class EventQueue {
private:
	concurrent_queue<std::unique_ptr<GameEvent>> eventQueue;

	EventQueue() = default;
	EventQueue(const EventQueue&) = delete;
	EventQueue& operator=(const EventQueue&) = delete;

public:
	static EventQueue& GetInstance()
	{
		static EventQueue instance;
		return instance;
	}

	void PushEvent(std::unique_ptr<GameEvent> event);
	bool PopEvent(std::unique_ptr<GameEvent>& event);
	bool IsEventExist();
};