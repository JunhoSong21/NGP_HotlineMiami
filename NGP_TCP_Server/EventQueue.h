#pragma once
#include "concurrentqueue.h"
#include <memory>

#include "GameEvent.h"

class EventQueue {
private:
	moodycamel::ConcurrentQueue<std::unique_ptr<GameEvent>> eventQueue;

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
};