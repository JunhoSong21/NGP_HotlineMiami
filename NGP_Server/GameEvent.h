#pragma once
#include <memory>

struct GameEvent {
	enum Type {
		KEY_INPUT,
		BULLET_TRIGGER,
		GRENADE_THROW,
		
		LOGIN_TRY,
		ROOM_MAKE,
		ROOM_READY,
	};

	Type type;
	int clientThreadId;

	virtual ~GameEvent() = default;
};

struct PlayerMove : GameEvent {
	float destX, destY;

	PlayerMove(int threadId, float x, float y) :
		destX(x), destY(y)
	{
		type = GameEvent::Type::KEY_INPUT;
		clientThreadId = threadId;
	}
};

struct BulletTrigger : GameEvent {
	float posX, posY;

	BulletTrigger(int threadId, float x, float y) :
		posX(x), posY(y)
	{
		type = GameEvent::Type::BULLET_TRIGGER;
		clientThreadId = threadId;
	}
};

struct GrenadeThrow : GameEvent {
	float posX, posY;

	GrenadeThrow(int threadId, float x, float y) :
		posX(x), posY(y)
	{
		type = GameEvent::Type::GRENADE_THROW;
		clientThreadId = threadId;
	}
};