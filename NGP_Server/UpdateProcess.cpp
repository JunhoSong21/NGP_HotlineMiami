#include "UpdateProcess.h"
#include "Common.h"
#include "GameEvent.h"
#include "EventManager.h"
#include "ThreadManager.h"
#include "Packet.h"

UpdateProcess::UpdateProcess()
{
}

UpdateProcess::~UpdateProcess()
{
}

void UpdateProcess::EventProcess()
{
	std::unique_ptr<GameEvent> gameEvent;

	while (EventManager::GetInstance().PopEvent(gameEvent)) {
		switch (gameEvent->type) {
		case GameEvent::Type::PLAYER_INPUT:
			ProcessPlayerInput();
			break;
		case GameEvent::Type::BULLET_INPUT:
			ProcessBulletInput();
			break;
		case GameEvent::Type::GRENADE_INPUT:
			ProcessGrenadeInput();
			break;
		default:
			printf("정의되지 않은 이벤트 타입이 있습니다.\n");
			break;
		}
	}
}

void UpdateProcess::ProcessPlayerInput()
{

}

void UpdateProcess::ProcessBulletInput()
{

}

void UpdateProcess::ProcessGrenadeInput()
{

}