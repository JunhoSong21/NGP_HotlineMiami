#pragma once
#include "Player.h"
#include "Bullet.h"

class Player;
int Send_Input(SOCKET sock, HWND hWnd, const Player& player);

// 전역 네트워크 상태
extern SOCKET g_ClientSock;
extern bool   g_NetworkRunning;
extern int    g_MyPlayerIndex;


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
