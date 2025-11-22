#pragma once
#include <cstdint>

#pragma pack(push, 1)

struct PacketHeader
{
	int     packetType;   // 어떤 패킷인지 
	size_t  packetSize;   // 이 패킷 전체 크기
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
