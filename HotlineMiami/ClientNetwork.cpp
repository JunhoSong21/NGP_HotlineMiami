#include "pch.h"
#include "ClientNetwork.h"
#include "NetProtocol.h"
#include "Player.h"

SOCKET g_ClientSock = INVALID_SOCKET;
bool   g_NetworkRunning = false;
extern GrenadeRequest g_GrenadeReq{ false, 0.0f }; // 수류탄 요청

// 수류탄 패킷 전송 함수
static int Send_GrenadeThrow(SOCKET sock, float dirRadAngle)
{
    CS_GRENADE_THROW pkt{};
    pkt.dirRadAngle = dirRadAngle;

    PacketHeader header{};
    header.packetType = PN::CS_GRENADE_THROW;
    header.packetSize = sizeof(pkt);

    // 헤더 전송
    int sent = send(
        sock,
        reinterpret_cast<const char*>(&header),
        static_cast<int>(sizeof(header)),
        0
    );
    if (sent == SOCKET_ERROR) {
        return SOCKET_ERROR;
    }

    // 본문 전송
    sent = send(
        sock,
        reinterpret_cast<const char*>(&pkt),
        static_cast<int>(sizeof(pkt)),
        0
    );
    if (sent == SOCKET_ERROR) {
        return SOCKET_ERROR;
    }

    return static_cast<int>(sizeof(header) + sizeof(pkt));
}

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

    // 수류탄 요청이 있으면 같이 보내기 
    if (g_GrenadeReq.requested) {
        if (Send_GrenadeThrow(sock, g_GrenadeReq.dirRadAngle) == SOCKET_ERROR) {
            return SOCKET_ERROR;
        }
        g_GrenadeReq.requested = false;
    }

    OutputDebugStringA("send!\n");

    return static_cast<int>(sizeof(header) + sizeof(pkt));

}

void RecvProcess(SOCKET sock)
{
    PacketHeader header{};
    int retValue = recv(sock, (char*)&header, sizeof(header), 0);
    if (retValue == SOCKET_ERROR) {}; // 오류시 처리코드 추가 필요

    switch (header.packetType) {
    case PN::SC_PLAYER_MOVE: {
        SC_PLAYER_MOVE playerMovePacket{};
        retValue = recv(sock, (char*)&playerMovePacket, sizeof(playerMovePacket), MSG_WAITALL);
        if (retValue == SOCKET_ERROR) {}; // 오류시 처리코드 추가 필요

        // playerMovePacket에 담긴 데이터로 플레이어 객체 업데이트
        // 기본적으로 클라이언트가 3개라는 가정을 하고 전체적인 구조를 수정 할 필요가 있음
        // Player 클래스 내부에 포지션을 변경할 수 있는 내부 함수 구현 필요
        break;
    }
    }


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

    while (g_NetworkRunning)
    {
        // 입력을 서버에 보냄
        if (Send_Input(g_ClientSock, hWnd, *player) == SOCKET_ERROR)
        {
            g_NetworkRunning = false;
            break;
        }

        //(나중에) RecvProcess();
        Sleep(16);
    }

    delete p; // 동적 할당했던 param delete
    return 0;
}
