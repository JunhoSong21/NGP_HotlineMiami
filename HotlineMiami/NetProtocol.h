#pragma once
#include <cstdint>



struct CS_LOGIN_PACKET		// 접속을 시도하는 클라이언트 정보 패킷
{
	char playerID[20];
};

struct CS_ROOM_PACKET		// 방 관련 정보를 전송하는 패킷
{
	bool createNewRoom;		// true: 새 방 생성
	int ready;				// 0: 준비 안함, 1: 준비 완료
};

struct CS_KEY_INPUT			// 키보드 입력 정보를 전송하는 패킷
{
	uint16_t flags;			// 입력 플래그 (WASD, FIRE 등)
	float posX;				// 플레이어 X좌표
	float posY;				// 플레이어 Y좌표
	float mouseX;
	float mouseY;
};

struct CS_BULLET_TRIGGER	// 총알 발사를 알리는 패킷
{
	float dirRad;
};

struct CS_GRENADE_THROW		// 수류탄 투척을 알리는 패킷
{
	float dirRad;
};
