#pragma once

struct GameEvent {
	enum Type {
		PLAYER_MOVE,
		PLAYER_UPDATE,
		BULLET_TRIGGER,
		BULLET_UPDATE,
		BULLET_COLLISION,
		GRENADE_THROW,
		GRENADE_UPDATE,
		GRENADE_EXPLOSION,

		LOGIN_TRY,
		ROOM_MAKE,
		ROOM_READY,
	};

	Type	type;
	int		networkThreadId;

	GameEvent() : type(PLAYER_MOVE), networkThreadId(-1) {}
	virtual ~GameEvent() = default;
	GameEvent(const GameEvent&) = delete;
	GameEvent& operator=(const GameEvent&) = delete;
};

struct PlayerMove : GameEvent {
	float destX, destY, angleX, angleY;

	PlayerMove(int threadId, float x, float y, float aX, float aY) :
		destX(x), destY(y), angleX(aX), angleY(aY)
	{
		type = GameEvent::Type::PLAYER_MOVE;
		networkThreadId = threadId;
	}
};

struct PlayerUpdate : GameEvent {
	PlayerUpdate()
	{
		type = GameEvent::Type::PLAYER_UPDATE;
		networkThreadId = 0;
	}
};

struct BulletTrigger : GameEvent {
	float posX, posY, atan2;

	BulletTrigger(int threadId, float x, float y, float angle) :
		posX(x), posY(y), atan2(angle)
	{
		type = GameEvent::Type::BULLET_TRIGGER;
		networkThreadId = threadId;
	}
};

struct BulletUpdate : GameEvent {
	BulletUpdate()
	{
		type = GameEvent::Type::BULLET_UPDATE;
		networkThreadId = 0;
	}
};

struct BulletCollision : GameEvent {
	bool isActive;

	BulletCollision(int threadId, bool isCollision) :
		isActive(isCollision)
	{
		type = GameEvent::Type::BULLET_COLLISION;
		networkThreadId = threadId;
	}
};

struct GrenadeThrow : GameEvent {
	float dirAngle;

	GrenadeThrow(int threadId, float angle) :
		dirAngle(angle)
	{
		type = GameEvent::Type::GRENADE_THROW;
		networkThreadId = threadId;
	}
};

//struct GrenadeUpdate : GameEvent {
//
//};

struct GrenadeExplosion : GameEvent {
	bool isActive;

	GrenadeExplosion(int threadId) :
		isActive(false)
	{
		type = GameEvent::Type::GRENADE_EXPLOSION;
		networkThreadId = threadId;
	}
};

//struct LoginTry : GameEvent {
//
//};
//
//struct RoomMake : GameEvent {
//
//};
//
//struct RoomReady : GameEvent {
//
//};