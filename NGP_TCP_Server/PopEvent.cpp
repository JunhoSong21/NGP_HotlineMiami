#include "PopEvent.h"

#include <stdio.h>

#include "DataManager.h"
#include "Player.h"

void PopEvent::HandleEvent(std::unique_ptr<GameEvent> event)
{
	if (!event)
		printf("nullptr Event\n");

	switch (event->type) {
	case GameEvent::Type::PLAYER_MOVE: {
		std::unique_ptr<PlayerMove> moveEvent(static_cast<PlayerMove*>(event.release()));
		HandlePlayerMoveEvent(std::move(moveEvent));
		break;
	}
	default:
		break;
	}
}

void PopEvent::HandlePlayerMoveEvent(std::unique_ptr<PlayerMove> event)
{
	Player* player = DataManager::GetInstance().GetPlayer(event->networkThreadId);
	if (player) {
		player->posX = event->destX;
	}
	printf("playerMoveEvent 처리 완료\n");
}