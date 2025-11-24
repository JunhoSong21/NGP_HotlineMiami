#include "PopEvent.h"

#include <cmath>
constexpr float PI = 3.1415;

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
	std::lock_guard<std::mutex> lock(playerUpdateMutex);

	Player* player = DataManager::GetInstance().GetPlayer(event->networkThreadId);
	if (player) {
		player->posX = event->destX;
		player->posY = event->destY;
		player->angle = CalculateAtan2Float(
			event->angleX, event->angleY, event->destX, event->destY);
	}
	printf("playerMoveEvent 처리 완료\n");
}

float PopEvent::CalculateAtan2Float(float x1, float y1, float x2, float y2)
{
	return atan2f(y2 - y1, x2 - x1) * 180.f / PI;
}