#pragma once
#include <memory>

struct GameEvent {
	enum Type {
		PLAYER_INPUT,
		BULLET_INPUT,
		GRENADE_INPUT,
		
		LOGIN_TRY,
		ROOM_MAKE,
		ROOM_READY,
	};

	Type type;

	virtual ~GameEvent() = default;
};