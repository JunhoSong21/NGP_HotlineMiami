#pragma once

struct GameEvent {
	enum Type {
		PLAYER_MOVE,
		BULLET_TRIGGER,
		BULLET_UPDATE,
		GRENADE_THROW,
		GRENADE_UPDATE,

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

};

struct BulletUpdate : GameEvent {

};

struct GrenadeThrow : GameEvent {

};

struct GrenadeUpdate : GameEvent {

};

struct LoginTry : GameEvent {

};

struct RoomMake : GameEvent {

};

struct RoomReady : GameEvent {

};