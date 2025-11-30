#include "EventQueue.h"

using std::unique_ptr;

void EventQueue::PushEvent(unique_ptr<GameEvent> event)
{
	eventQueue.enqueue(std::move(event));
}

bool EventQueue::PopEvent(unique_ptr<GameEvent>& event)
{
	return eventQueue.try_enqueue(std::move(event));
}