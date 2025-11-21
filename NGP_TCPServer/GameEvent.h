#pragma once

struct GameEvent {
	enum Type {
		PLAYER_MOVE,
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

	virtual ~GameEvent() = default;
};

struct PlayerMove : GameEvent {
	float destX, destY;

	PlayerMove(int threadId, float x, float y) :
		destX(x), destY(y)
	{
		type = GameEvent::Type::PLAYER_MOVE;
		networkThreadId = threadId;
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

struct GrenadeThrow : GameEvent {

};

struct GrenadeUpdate : GameEvent {

};

struct GrenadeExplosion : GameEvent {

};

struct LoginTry : GameEvent {

};

struct RoomMake : GameEvent {

};

struct RoomReady : GameEvent {

};