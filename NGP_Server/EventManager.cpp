#include "EventManager.h"

void EventManager::PushEvent(std::unique_ptr<GameEvent> event)
{
	EventQueue.push(std::move(event));
}

bool EventManager::PopEvent(std::unique_ptr<GameEvent>& event)
{
	return EventQueue.try_pop(event);
}

bool EventManager::IsInEvent()
{
	return !EventQueue.empty();
}