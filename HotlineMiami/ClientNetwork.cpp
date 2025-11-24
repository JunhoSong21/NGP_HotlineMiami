#include "pch.h"
#include "ClientNetwork.h"
#include "NetProtocol.h"
#include "Player.h"
#include "Bullet.h"

SOCKET g_ClientSock = INVALID_SOCKET;
bool   g_NetworkRunning = false;

int Send_Input(SOCKET sock, HWND hWnd, const Player& player)
{
    OutputDebugStringA("sendready\n");
    CS_KEY_INPUT pkt{};
    pkt.flags = 0;


    // 키보드 상태
    if (GetAsyncKeyState('W') & 0x8000) pkt.flags |= KEY_MOVE_W;
    if (GetAsyncKeyState('A') & 0x8000) pkt.flags |= KEY_MOVE_A;
    if (GetAsyncKeyState('S') & 0x8000) pkt.flags |= KEY_MOVE_S;
    if (GetAsyncKeyState('D') & 0x8000) pkt.flags |= KEY_MOVE_D;

    // 플레이어 현재 위치
    const Gdiplus::PointF& pos = player.GetPosition();
    pkt.posX = pos.X;
    pkt.posY = pos.Y;

    // 마우스 위치
    POINT mouse;
    GetCursorPos(&mouse);
    ScreenToClient(hWnd, &mouse);

    pkt.mouseX = static_cast<float>(mouse.x);
    pkt.mouseY = static_cast<float>(mouse.y);

    // 패킷헤더 채우기
    PacketHeader header{};
    header.packetType = PN::CS_KEY_INPUT;
    header.packetSize = sizeof(PacketHeader) + sizeof(pkt);
    
    // 헤더 먼저 보내기
    int sent = send(sock, reinterpret_cast<const char*>(&header),
        sizeof(header), 0);
    if (sent == SOCKET_ERROR)
        return SOCKET_ERROR;
    // 바디 보내기
    sent = send(
        sock,
		reinterpret_cast<const char*>(&pkt),    // reinterpret_cast는 포인터 타입 변환에 사용된다
        static_cast<int>(sizeof(pkt)),
        0
    );
    if (sent == SOCKET_ERROR)
        return SOCKET_ERROR;

    OutputDebugStringA("send!\n");

    return static_cast<int>(sizeof(header) + sizeof(pkt));

}

// 네트워크 초기화
bool InitNetwork(const char* serverIp, unsigned short port)
{
	// 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return false;
	// 소켓 생성
    g_ClientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (g_ClientSock == INVALID_SOCKET)
        return false;
	// connect
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, serverIp, &server.sin_addr);

    if (connect(g_ClientSock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
        return false;

    g_NetworkRunning = true;
    return true;
}

// 네트워크 종료
void ShutdownNetwork()
{
    g_NetworkRunning = false;

    if (g_ClientSock != INVALID_SOCKET)
    {
        closesocket(g_ClientSock);
        g_ClientSock = INVALID_SOCKET;
    }
    WSACleanup();
}

DWORD WINAPI Client_Network_Thread(LPVOID param)
{
    NetworkThreadParam* p = reinterpret_cast<NetworkThreadParam*>(param);
    HWND    hWnd = p->hWnd;
    Player* player = p->player;
	Bullet* bullet = p->bullet;

    while (g_NetworkRunning)
    {
        // 입력을 서버에 보냄
        if (Send_Input(g_ClientSock, hWnd, *player) == SOCKET_ERROR)
        {
            g_NetworkRunning = false;
            break;
        }
        
        // RecvProcess();
        if (RecvProcess(g_ClientSock, bullet) == -1)
        {
            g_NetworkRunning = false;
            break;
        }


        Sleep(16);
    }

    delete p; // 동적 할당했던 param delete
    return 0;
}

// Recv 관련 함수 구현
int Recv_BulletData(SOCKET sock, Bullet* bullet)
{
    if (!bullet)
        return 0;

    SC_BULLET_STATE pkt{};
    int ret = recv(sock, reinterpret_cast<char*>(&pkt),
        sizeof(pkt), MSG_WAITALL);
    if (ret <= 0)
    {
        return -1;
    }

    bullet->SetVisible(pkt.isActive != 0);
    bullet->SetPosition(pkt.posX, pkt.posY);
    bullet->SetDirection(pkt.dirX, pkt.dirY);

    return ret;
}

int RecvProcess(SOCKET sock, Bullet* bullet)
{
    // 공통 헤더 먼저 받기
    PacketHeader header{};
    int ret = recv(sock, reinterpret_cast<char*>(&header),
        sizeof(header), MSG_WAITALL);
    if (ret <= 0)
    {
        return -1;  // 소켓 종료/에러
    }

    // 2) 패킷 종류에 따라 분기
    switch (header.packetType)
    {
    case PN::SC_BULLET_STATE:
        return Recv_BulletData(sock, bullet);

    default:
        return 0;
    }
}

