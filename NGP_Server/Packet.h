#pragma once
#include <cstdint>

#pragma pack(1)
struct PacketHeader {
	int packetType;
	size_t packetSize;
};

enum KEY_INPUT_FLAGS : uint16_t {
	KEY_MOVE_W = 1 << 0,
	KEY_MOVE_A = 1 << 1,
	KEY_MOVE_S = 1 << 2,
	KEY_MOVE_D = 1 << 3,
};

// Server

struct SC_PLAYER_MOVE {
	float posX;		// X ÁÂÇ¥
	float posY;		// Y ÁÂÇ¥
	float atan2;	// È¸Àü°ª
};

struct SC_BULLET_STATE {
	bool isVisible;	// È°¼º ¿©ºÎ
	float posX;		// X ÁÂÇ¥
	float posY;		// Y ÁÂÇ¥
	float dirX;		// X º¤ÅÍ
	float dirY;		// Y º¤ÅÍ
};

struct SC_GRENADE_STATE {
	bool isVisible;	// È°¼º ¿©ºÎ
	float posX;		// X ÁÂÇ¥
	float posY;		// Y ÁÂÇ¥
	float dirX;		// X º¤ÅÍ
	float dirY;		// Y º¤ÅÍ
	bool isExploded;// Æø¹ß ¿©ºÎ
};

struct SC_GAME_END {
	bool isMatchEnded;	// °ÔÀÓ Á¾·á ¿©ºÎ
};

// Client

struct CS_LOGIN_PACKET {
	char playerID[20];
};

struct CS_ROOM_PACKET {
	bool isCreateNewRoon;
	bool isReady;
};

struct CS_KEY_INPUT {
	uint16_t flags;
	float posX;
	float posY;
	float mouseX;
	float mouseY;
};

struct CS_BULLET_TRIGGER {
	float dirRad;
};

struct CS_GRENADE_THROW {
	float dirRad;
};

#pragma pack()

namespace PN { // ³×ÀÓ½ºÆäÀÌ½º
	constexpr int SC_PLAYER_MOVE	= 1000;
	constexpr int SC_BULLET_STATE	= 1001;
	constexpr int SC_GRENADE_STATE	= 1002;
	constexpr int SC_GAME_END		= 1003;
	constexpr int CS_LOGIN_PACKET	= 1004;
	constexpr int CS_ROOM_PACKET	= 1005;
	constexpr int CS_KEY_INPUT		= 1006;
	constexpr int CS_BULLET_TRIGGER = 1007;
	constexpr int CS_GRENADE_THROW	= 1008;
}