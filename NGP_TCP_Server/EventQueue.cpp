#include "EventQueue.h"

void EventQueue::PushEvent(std::unique_ptr<GameEvent> event)
{
	eventQueue.push(std::move(event));
}

bool EventQueue::PopEvent(std::unique_ptr<GameEvent>& event)
{
	return eventQueue.try_pop(event);
}

bool EventQueue::IsEventExist()
{
	return !eventQueue.empty();
}