#include "EventQueue.h"

void EventQueue::PushEvent(std::unique_ptr<GameEvent> event)
{
	eventQueue.enqueue(std::move(event));
}

bool EventQueue::PopEvent(std::unique_ptr<GameEvent>& event)
{
	return eventQueue.try_enqueue(std::move(event));
}