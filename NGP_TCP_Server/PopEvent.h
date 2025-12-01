#pragma once
#include <memory>
#include <mutex>

#include "GameEvent.h"
#include "ThreadManager.h"

class PopEvent {
public:
	PopEvent() = default;
	~PopEvent() = default;

	void HandleEvent(std::unique_ptr<GameEvent> event);

private:
	std::mutex playerMoveMutex;
	std::mutex playerUpdateMutex;

	std::mutex bulletUpdateMutex;

	void HandlePlayerMoveEvent(std::unique_ptr<PlayerMove> event);
	void HandlePlayerUpdateEvent(std::unique_ptr<PlayerUpdate> event);

	float CalculateAtan2Float(float x1, float y1, float x2, float y2);
};