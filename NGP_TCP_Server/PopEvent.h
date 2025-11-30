#pragma once
#include <memory>
#include <mutex>

#include "GameEvent.h"

class PopEvent {
public:
	PopEvent() = default;
	~PopEvent() = default;

	void HandleEvent(std::unique_ptr<GameEvent> event);

private:
	std::mutex playerUpdateMutex;
	std::mutex bulletUpdateMutex;
	void HandlePlayerMoveEvent(std::unique_ptr<PlayerMove> event);

	float CalculateAtan2Float(float x1, float y1, float x2, float y2);
};