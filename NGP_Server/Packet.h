#pragma once

struct SC_PLAYER_MOVE {
	float posX;		// X 좌표
	float posY;		// Y 좌표
	float atan2;	// 회전값
};

struct SC_BULLET_STATE {
	bool isVisible;	// 활성 여부
	float posX;		// X 좌표
	float posY;		// Y 좌표
	float dirX;		// X 벡터
	float dirY;		// Y 벡터
};

struct SC_GRENADE_STATE {
	bool isVisible;	// 활성 여부
	float posX;		// X 좌표
	float posY;		// Y 좌표
	float dirX;		// X 벡터
	float dirY;		// Y 벡터
	bool isExploded;// 폭발 여부
};

struct SC_GAME_END {
	bool isMatchEnded;	// 게임 종료 여부
};