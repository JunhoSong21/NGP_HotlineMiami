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
	float destX, destY;

	BulletUpdate(int threadId, float x, float y) :
		destX(x), destY(y)
	{
		type = GameEvent::Type::BULLET_UPDATE;
		networkThreadId = threadId;
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

//struct GrenadeThrow : GameEvent {
//
//};
//
//struct GrenadeUpdate : GameEvent {
//
//};
//
//struct GrenadeExplosion : GameEvent {
//
//};
//
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