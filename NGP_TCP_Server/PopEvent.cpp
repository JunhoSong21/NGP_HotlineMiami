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
	case GameEvent::Type::BULLET_TRIGGER: {
		unique_ptr<BulletTrigger> bulletTriggerEvent(static_cast<BulletTrigger*>(event.release()));
		HandleBulletTriggerEvent(std::move(bulletTriggerEvent));
		break;
	}
	case GameEvent::Type::BULLET_UPDATE: {
		unique_ptr<BulletUpdate> bulletUpdateEvent(static_cast<BulletUpdate*>(event.release()));
		HandleBulletUpdateEvent(std::move(bulletUpdateEvent));
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
	if (player && (player->GetHp() > 0.0f)) {
		player->CalcPosbyFlag(event->flag, event->posX, event->posY);
		player->SetAngle(
			atan2f(event->mouseY - event->posY, event->mouseX - event->posX));  // radian
	}

#ifdef _DEBUG
	printf("playerMoveEvent 처리 완료\n");
#endif
}

void PopEvent::HandlePlayerUpdateEvent(unique_ptr<PlayerUpdate> event)
{
	lock_guard<mutex> lock(playerUpdateMutex);

	ThreadManager::GetInstance().BroadcastEvent(std::move(event));
}

void PopEvent::HandleBulletTriggerEvent(unique_ptr<BulletTrigger> event)
{
	lock_guard<mutex> lock(bulletTriggerMutex);

	if (DataManager::GetInstance().GetBullet(event->networkThreadId) != nullptr) {
		// 이미 해당 클라이언트 번호로 총알이 map에 들어가 있을 경우
		Player* rootPlayer = DataManager::GetInstance().GetPlayer(event->networkThreadId);
		Bullet* bullet = DataManager::GetInstance().GetBullet(event->networkThreadId);
		bullet->SetIsActive(true);
		bullet->SetPos(event->posX, event->posY);
		bullet->SetAngle(event->atan2);
	}
	else {
		Player* rootPlayer = DataManager::GetInstance().GetPlayer(event->networkThreadId);
		unique_ptr<Bullet> newBullet = make_unique<Bullet>(
			event->networkThreadId,
			rootPlayer->GetPosX(),
			rootPlayer->GetPosY(),
			rootPlayer->GetAngle()
		);

		DataManager::GetInstance().AddBullet(std::move(newBullet));
	}
	
#ifdef _DEBUG
	printf("bulletTriggerEvent 처리 완료\n");
#endif
}

void PopEvent::HandleBulletUpdateEvent(unique_ptr<BulletUpdate> event)
{
	lock_guard<mutex> lock(bulletUpdateMutex);

	ThreadManager::GetInstance().BroadcastEvent(std::move(event));
}

void PopEvent::HandleGrenadeThrowEvent(unique_ptr<GrenadeThrow> event)
{
	lock_guard<mutex> lock(grenadeThrowMutex);

	unique_ptr<Grenade> newGrenade = make_unique<Grenade>(
		event->networkThreadId,
		event->posX,
		event->posY
	);

	DataManager::GetInstance().AddGrenade(std::move(newGrenade));
	Timer::GetInstance().AddGrenade(event->networkThreadId);
	ThreadManager::GetInstance().BroadcastEvent(std::move(event));

#ifdef _DEBUG
	printf("grenadeThrowEvent 처리 완료\n");
#endif
}

void PopEvent::HandleGrenadeExplosionEvent(unique_ptr<GrenadeExplosion> event)
{
	lock_guard<mutex> lock(grenadeExplosionMutex);

	// 수류탄을 폭발 상태로 전환 후 이벤트 브로드캐스트
	Grenade* grenade = DataManager::GetInstance().GetGrenade(event->networkThreadId);
	grenade->ChangeStateToExplode();

	ThreadManager::GetInstance().BroadcastEvent(std::move(event));
}

float PopEvent::CalculateAtan2Float(float x1, float y1, float x2, float y2)
{
	return atan2f(y2 - y1, x2 - x1) * 180.f / PI;
}