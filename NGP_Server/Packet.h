#pragma once

#pragma pack(1)
struct PacketHeader {
	int packetType;
	size_t packetSize;
};

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

#pragma pack()

namespace PN {
	constexpr int SC_PLAYER_MOVE	= 1000;
	constexpr int SC_BULLET_STATE	= 1001;
	constexpr int SC_GRENADE_STATE	= 1002;
	constexpr int SC_GAME_END		= 1003;
}