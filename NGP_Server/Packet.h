#pragma once

struct PacketHeader {
	int packetType;
	size_t packetSize;
};

constexpr int SC_PLAYER_MOVE = 1000;
struct SC_PLAYER_MOVE {
	float posX;		// X 좌표
	float posY;		// Y 좌표
	float atan2;	// 회전값
};

constexpr int SC_BULLET_STATE = 1001;
struct SC_BULLET_STATE {
	bool isVisible;	// 활성 여부
	float posX;		// X 좌표
	float posY;		// Y 좌표
	float dirX;		// X 벡터
	float dirY;		// Y 벡터
};

constexpr int SC_GRENADE_STATE = 1002;
struct SC_GRENADE_STATE {
	bool isVisible;	// 활성 여부
	float posX;		// X 좌표
	float posY;		// Y 좌표
	float dirX;		// X 벡터
	float dirY;		// Y 벡터
	bool isExploded;// 폭발 여부
};

constexpr int SC_GAME_END = 1003;
struct SC_GAME_END {
	bool isMatchEnded;	// 게임 종료 여부
};