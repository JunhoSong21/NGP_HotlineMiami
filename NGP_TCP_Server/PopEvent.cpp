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
	case GameEvent::Type::GRENADE_THROW: {
		unique_ptr<GrenadeThrow> grenadeThrowEvent(static_cast<GrenadeThrow*>(event.release()));
		HandleGrenadeThrowEvent(std::move(grenadeThrowEvent));
		break;
	}
	case GameEvent::Type::GRENADE_EXPLOSION: {
		unique_ptr<GrenadeExplosion> grenadeExplosionEvent(static_cast<GrenadeExplosion*>(event.release()));
		HandleGrenadeExplosionEvent(std::move(grenadeExplosionEvent));
		break;
	}
	default:
		printf("Undefine Event Type\n");
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
#ifdef _DEBUG
	printf("playerMoveEvent 贸府 肯丰\n");
#endif
}

void PopEvent::HandlePlayerUpdateEvent(unique_ptr<PlayerUpdate> event)
{
	lock_guard<mutex> lock(playerUpdateMutex);

	ThreadManager::GetInstance().BroadcastEvent(std::move(event));
}

void PopEvent::HandleGrenadeThrowEvent(unique_ptr<GrenadeThrow> event)
{
	lock_guard<mutex> lock(grenadeThrowMutex);

	Player* rootPlayer = DataManager::GetInstance().GetPlayer(event->networkThreadId);

	auto newGrenade = make_unique<Grenade>(
		event->networkThreadId,
		rootPlayer->posX,
		rootPlayer->posY
	);

	Timer::GetInstance().AddGrenade(event->networkThreadId);
	printf("grenadeThrowEvent 贸府 肯丰\n");
}

void PopEvent::HandleGrenadeExplosionEvent(unique_ptr<GrenadeExplosion> event)
{
	lock_guard<mutex> lock(grenadeExplosionMutex);

	ThreadManager::GetInstance().BroadcastEvent(std::move(event));
}

float PopEvent::CalculateAtan2Float(float x1, float y1, float x2, float y2)
{
	return atan2f(y2 - y1, x2 - x1) * 180.f / PI;
}