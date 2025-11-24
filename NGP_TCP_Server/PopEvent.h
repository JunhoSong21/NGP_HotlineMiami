#pragma once
#include <memory>

#include "GameEvent.h"

class PopEvent {
public:
	PopEvent() = default;
	~PopEvent() = default;

	void HandleEvent(std::unique_ptr<GameEvent> event);

private:
	void HandlePlayerMoveEvent(std::unique_ptr<PlayerMove> event);
};