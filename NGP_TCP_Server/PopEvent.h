#pragma once
#include <memory>
#include <mutex>

#include "GameEvent.h"
#include "ThreadManager.h"
#include "Timer.h"

class PopEvent {
public:
	PopEvent() = default;
	~PopEvent() = default;

	void HandleEvent(std::unique_ptr<GameEvent> event);

private:
	std::mutex playerMoveMutex;
	std::mutex playerUpdateMutex;

	std::mutex bulletTriggerMutex;
	std::mutex bulletUpdateMutex;

	std::mutex grenadeThrowMutex;
	std::mutex grenadeExplosionMutex;
	std::mutex grenadeUpdateMutex;

	void HandlePlayerMoveEvent(std::unique_ptr<PlayerMove> event);
	void HandlePlayerUpdateEvent(std::unique_ptr<PlayerUpdate> event);

	void HandleBulletTriggerEvent(std::unique_ptr<BulletTrigger> event);
	void HandleBulletUpdateEvent(std::unique_ptr<BulletUpdate> event);

	void HandleGrenadeThrowEvent(std::unique_ptr<GrenadeThrow> event);
	void HandleGrenadeExplosionEvent(std::unique_ptr<GrenadeExplosion> event);
	void HandleGrenadeUpdateEvent(std::unique_ptr<GrenadeUpdate> event);
	
	void HandleGameEndEvent(std::unique_ptr<GameEnd> event);

	float CalculateAtan2Float(float x1, float y1, float x2, float y2);
};