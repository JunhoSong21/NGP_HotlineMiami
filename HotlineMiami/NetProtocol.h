#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct PacketHeader {
	int32_t		packetType;
	uint16_t 	packetSize;
};

// Server > Client
struct SC_PLAYER_MOVE {
	short	targetNum;

	float	posX;
	float	posY;
	float	angle;
};

struct SC_BULLET_STATE {
	short	targerNum;

	bool	isActive;
	float	posX;
	float	posY;
	float	dirAngle;
};

struct SC_GRENADE_STATE {
	short	targetNum;

	bool	isExploded;
	float	posX;
	float	posY;
	float	dirX;
	float	dirY;
};

struct SC_LOGIN_SUCCESS {
	bool	isSuccess;
	int		targetNum;
};

struct SC_GAME_END {
	bool	isMatchEnded;
};

// Client > Server
struct CS_LOGIN_PACKET {
	char	clientIp[16];
};

struct CS_ROOM_PACKET {
	bool	isCreateNewRoom;
	bool	isReady;
};

struct CS_KEY_INPUT {
	uint16_t flags;
	float	posX;
	float	posY;
	float	mouseX;
	float	mouseY;
};

struct CS_BULLET_TRIGGER {
	float	posX;
	float	posY;
	float	dirRadAngle;
};

struct CS_GRENADE_THROW {
	float	dirRadAngle;
};
#pragma pack(pop)

namespace PN { // Packet Name
	constexpr int SC_PLAYER_MOVE = 1000;
	constexpr int SC_BULLET_STATE = 1001;
	constexpr int SC_GRENADE_STATE = 1002;
	constexpr int SC_LOGIN_SUCCESS = 1003;
	constexpr int SC_GAME_END = 1004;
	constexpr int CS_LOGIN_PACKET = 1005;
	constexpr int CS_ROOM_PACKET = 1006;
	constexpr int CS_KEY_INPUT = 1007;
	constexpr int CS_BULLET_TRIGGER = 1008;
	constexpr int CS_GRENADE_THROW = 1009;
}

enum KEY_INPUT_FLAGS : uint16_t		//  키 입력 플래그 정의
{
	KEY_MOVE_W = 1 << 0,
	KEY_MOVE_A = 1 << 1,
	KEY_MOVE_S = 1 << 2,
	KEY_MOVE_D = 1 << 3,
};