#include "PopEvent.h"

#include <cmath>
constexpr double PI = 3.1415;

#include "DataManager.h"
#include "Player.h"
#include "Wall.h"

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
	case GameEvent::Type::GAME_END: {
		unique_ptr<GameEnd> gameEndEvent(static_cast<GameEnd*>(event.release()));
		HandleGameEndEvent(std::move(gameEndEvent));
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

		const float oldX = player->GetPosX();
		const float oldY = player->GetPosY();

		float x = oldX;
		float y = oldY;

		// 기존 이동 계산 (여기서 내부적으로 SetPosX/SetPosY 호출 중)
		player->CalcPosbyFlag(event->flag, x, y);

		float newX = player->GetPosX();
		float newY = player->GetPosY();

		// 클라에서 DebugRenderCollision 사각형이 35x35 이므로 반지름 17.5 정도로 사용
		constexpr float PLAYER_RADIUS = 17.5f;

		Wall::Get().ResolveMove(oldX, oldY, newX, newY, PLAYER_RADIUS);

		// 최종 위치를 서버 권위 값으로 확정
		player->SetPosX(newX);
		player->SetPosY(newY);

		// 회전 계산도 서버 좌표 기준으로
		player->SetAngle(
			CalculateAtan2Float(event->mouseX, event->mouseY, x, y));
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

	if (!DataManager::GetInstance().GetBullet(event->networkThreadId))
		return;
	
	// 이미 해당 클라이언트 번호로 총알이 map에 들어가 있을 경우
	Player* rootPlayer = DataManager::GetInstance().GetPlayer(event->networkThreadId);
	Bullet* bullet = DataManager::GetInstance().GetBullet(event->networkThreadId);
	bullet->SetIsActive(true);
	bullet->SetPos(event->posX, event->posY);
	bullet->SetAngle(event->atan2);
	
	#ifdef _DEBUG
	printf("bulletTriggerEvent 처리 완료\n");
	#endif
}

void PopEvent::HandleBulletUpdateEvent(unique_ptr<BulletUpdate> event)
{
	ThreadManager::GetInstance().BroadcastEvent(std::move(event));
}

void PopEvent::HandleGrenadeThrowEvent(std::unique_ptr<GrenadeThrow> event)
{
	// 1) 개인당 2발 제한 체크 (DataManager에 간단한 카운터 추가를 추천)
	if (!DataManager::GetInstance().TryConsumeGrenade(event->networkThreadId)) {
#ifdef _DEBUG
		printf("수류탄 개수 제한으로 인해 GRENADE_THROW 무시\n");
#endif
		return;
	}

	// 2) 실제 그레네이드 객체 가져오기
	Grenade* grenade = DataManager::GetInstance().GetGrenade(event->networkThreadId);
	if (!grenade) {
		return;
	}

	// 3) Throw(위치 + 각도) 호출 → 서버에서 궤도 계산
	grenade->Throw(event->posX, event->posY, event->dirAngle);

	// 4) 아직 퓨즈는 시작 안 함. (정지했을 때 내부 Update에서 퓨즈 시작)
	ThreadManager::GetInstance().BroadcastEvent(std::move(event));

#ifdef _DEBUG
	printf("grenadeThrowEvent 처리 완료\n");
#endif
}


void PopEvent::HandleGrenadeExplosionEvent(unique_ptr<GrenadeExplosion> event)
{
	// 수류탄을 폭발 상태로 전환 후 이벤트 브로드캐스트
	Grenade* grenade = DataManager::GetInstance().GetGrenade(event->networkThreadId);
	grenade->ChangeStateToExplode();

	ThreadManager::GetInstance().BroadcastEvent(std::move(event));
}

void PopEvent::HandleGameEndEvent(std::unique_ptr<GameEnd> event)
{
	ThreadManager::GetInstance().BroadcastEvent(std::move(event));
}

float PopEvent::CalculateAtan2Float(float x1, float y1, float x2, float y2)
{
	return atan2f(y2 - y1, x2 - x1);
}