#pragma once
#include "Player.h"
#include "Bullet.h"

class Player;
int Send_Input(SOCKET sock, HWND hWnd, const Player& player);

// 전역 네트워크 상태
extern SOCKET g_ClientSock;
extern bool   g_NetworkRunning;
extern int    g_MyPlayerIndex;

// 수류탄 요청 상태
struct GrenadeRequest {
    bool  requested;     // true면 이번 프레임에 서버로 보내야 함
    float dirRadAngle;   // 플레이어 기준 마우스 방향(라디안)
};

extern GrenadeRequest g_GrenadeReq;

// 서버통신 스레드용 파라미터 여러 값을 보내기 위해
struct NetworkThreadParam
{
    HWND    hWnd;
    Bullet* bullet;
    Player**    players;
};

// 초기화 / 종료
bool InitNetwork(const char* serverIp, unsigned short port);
void ShutdownNetwork();

// 서버 통신 스레드
void ShutdownNetwork();
DWORD WINAPI Client_Network_Thread(LPVOID param);
int RecvProcess(SOCKET sock, Bullet* bullet);
int Recv_BulletData(SOCKET sock, Bullet* bullet);
void RecvProcess(SOCKET sock, Player** players);
