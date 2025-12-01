#include "PopEvent.h"

#include <cmath>
constexpr float PI = 3.1415;

#include "DataManager.h"
#include "Player.h"

using std::unique_ptr;
using std::make_unique;
using std::lock_guard;
using std::mutex;

void PopEvent::HandleEvent(unique_ptr<GameEvent> event)
{
	if (!event)
		printf("nullptr Event\n");

	switch (event->type) {
	case GameEvent::Type::PLAYER_MOVE: {
		unique_ptr<PlayerMove> moveEvent(static_cast<PlayerMove*>(event.release()));
		HandlePlayerMoveEvent(std::move(moveEvent));
		break;
	}
	case GameEvent::Type::PLAYER_UPDATE: {
		unique_ptr<PlayerUpdate> playerUpdateEvent(static_cast<PlayerUpdate*>(event.release()));
		HandlePlayerUpdateEvent(std::move(playerUpdateEvent));
		break;
	}
	default:
		break;
	}
}

void PopEvent::HandlePlayerMoveEvent(unique_ptr<PlayerMove> event)
{
	lock_guard<mutex> lock(playerMoveMutex);

	Player* player = DataManager::GetInstance().GetPlayer(event->networkThreadId);
	if (player) {
		player->posX = event->destX;
		player->posY = event->destY;
		player->angle = CalculateAtan2Float(
			event->angleX, event->angleY, event->destX, event->destY);
	}
	printf("playerMoveEvent 처리 완료\n");
}

void PopEvent::HandlePlayerUpdateEvent(unique_ptr<PlayerUpdate> event)
{
	lock_guard<mutex> lock(playerUpdateMutex);

	ThreadManager::GetInstance().BroadcastEvent(std::move(event));
}

float PopEvent::CalculateAtan2Float(float x1, float y1, float x2, float y2)
{
	return atan2f(y2 - y1, x2 - x1) * 180.f / PI;
}