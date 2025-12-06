#include "pch.h"
#include "ClientNetwork.h"
#include "NetProtocol.h"
#include "Player.h"
#include "Bullet.h"

SOCKET g_ClientSock = INVALID_SOCKET;
bool   g_NetworkRunning = false;
int    g_MyPlayerIndex = -1;
extern GrenadeRequest g_GrenadeReq{ false, 0.0f }; // 수류탄 요청
BulletTriggerRequest g_BulletReq = { false, 0.0f };

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
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_pton(AF_INET, serverIp, &server.sin_addr);
    server.sin_port = htons(port);

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
    HWND        hWnd    = p->hWnd;
    Bullet*     bullet  = p->bullet;
    Player**    players = p->players;

    LoginProcess(g_ClientSock);

    while (g_NetworkRunning) {
        // 매 프레임마다 최신 내 인덱스 가져오기
        int idx = g_MyPlayerIndex;

        // 아직 서버가 내 인덱스를 안 알려줬으면 일단 0번으로 가정해서 테스트 가능
        if (idx < 0 || idx >= 3)
            idx = 0;

        Player* myPlayer = players[idx];

        if (myPlayer) {
            if (SendProcess(g_ClientSock, hWnd, *myPlayer) == SOCKET_ERROR) {
                g_NetworkRunning = false;
                break;
            }
        }
        
        RecvProcess(g_ClientSock, players, bullet);

        Sleep(16); // 디버그를 위한 초당 30번 전송
    }

    delete p; // 동적 할당했던 param delete
    return 0;
}

// 로그인 패킷 송수신
void LoginProcess(SOCKET sock)
{
    PacketHeader loginSendPacketHeader{};
    loginSendPacketHeader.packetType = PN::CS_LOGIN_PACKET;
    loginSendPacketHeader.packetSize = sizeof(CS_LOGIN_PACKET);

    CS_LOGIN_PACKET loginSendPacket{};
    //strncpy_s(loginSendPacket.clientIp, g_LoginReq.ip, sizeof(loginSendPacket.clientIp) - 1);

    int retValue = 0;
    retValue = send(sock, (char*)&loginSendPacketHeader, sizeof(loginSendPacketHeader), 0);
    if (retValue == SOCKET_ERROR)
        printf("로그인 패킷 헤더 전송 오류\n");

    retValue = send(sock, (char*)&loginSendPacket, sizeof(loginSendPacket), 0);
    if (retValue == SOCKET_ERROR)
        printf("로그인 패킷 전송 오류\n");

    PacketHeader loginRecvPacketHeader{};
    retValue = recv(sock, (char*)&loginRecvPacketHeader, sizeof(loginRecvPacketHeader), MSG_WAITALL);
    if (retValue == SOCKET_ERROR || loginRecvPacketHeader.packetType != PN::SC_LOGIN_SUCCESS)
        printf("로그인 패킷 헤더 수신 오류\n");

    SC_LOGIN_SUCCESS loginSuccess;
    retValue = recv(sock, (char*)&loginSuccess, sizeof(loginSuccess), MSG_WAITALL);
    if (retValue == SOCKET_ERROR || loginSuccess.isSuccess == false)
        printf("로그인 실패\n");
}

// Send 구조
int SendProcess(SOCKET sock, HWND hWnd, const Player& player)
{
    // 총알 요청이 있으면 총알부터
    if (g_BulletReq.requested) {
        
    }
    // 수류탄 요청이 있으면 수류탄부터
    else if (g_GrenadeReq.requested) {
        if (Send_GrenadeThrow(sock, g_GrenadeReq.dirRadAngle) == SOCKET_ERROR) {
            return SOCKET_ERROR;
        }
        g_GrenadeReq.requested = false;
    }
    // 아무 요청 없으면 키 상태 전송
    else {
        Send_Input(sock, hWnd, player);
    }

    return true;
}

// 키 상태 전송 함수
int Send_Input(SOCKET sock, HWND hWnd, const Player& player)
{
    // CS_KEY_INPUT
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
    header.packetSize = sizeof(pkt);

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

    return true;
}

// 수류탄 패킷 전송 함수
int Send_GrenadeThrow(SOCKET sock, float dirRadAngle)
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

// Recv 구조
void RecvProcess(SOCKET sock, Player** players, Bullet* bullet)
{
    PacketHeader header{};
    int retValue = recv(sock, (char*)&header, sizeof(header), MSG_WAITALL);
    if (retValue <= 0)
        return;

    switch (header.packetType) {
    case PN::SC_PLAYER_MOVE: {
        SC_PLAYER_MOVE playerMovePacket{};
        retValue = recv(sock, (char*)&playerMovePacket, sizeof(playerMovePacket), MSG_WAITALL);
        if (retValue <= 0)
            return;

        Recv_PlayerMove(players, playerMovePacket);
        break;
    }
    case PN::SC_GRENADE_STATE: {
        SC_BULLET_STATE bulletStatePacket{};
        retValue = recv(sock, (char*)&bulletStatePacket, sizeof(bulletStatePacket), MSG_WAITALL);
        if (retValue == SOCKET_ERROR)
            return;

        Recv_BulletData(bullet, bulletStatePacket);
        break;
    }
    default: {
        //// 아직 처리 안 하는 패킷은 bodySize만큼 읽어서 버리기
        //char dummy[256];
        //int remain = bodySize;
        //while (remain > 0)
        //{
        //    int chunk = (remain > (int)sizeof(dummy)) ? (int)sizeof(dummy) : remain;
        //    int r = recv(sock, dummy, chunk, 0);
        //    if (r <= 0) return;
        //    remain -= r;
        //}
        break;
    }
    }
}

void Recv_PlayerMove(Player** players, struct SC_PLAYER_MOVE playerMovePacket)
{
    int idx = static_cast<int>(playerMovePacket.targetNum);
    g_MyPlayerIndex = idx;
    if (idx >= 0 && idx < 3 && players[idx])
    {
        players[idx]->SetPosition(
            playerMovePacket.posX,
            playerMovePacket.posY,
            playerMovePacket.angle
        );
    }
}

int Recv_BulletData(Bullet* bullet, struct SC_BULLET_STATE bulletStatePacket)
{
    if (!bullet)
        return 0;

    bullet->SetVisible(bulletStatePacket.isActive != 0);
    bullet->SetPosition(bulletStatePacket.posX, bulletStatePacket.posY);
    bullet->SetDirection(bulletStatePacket.dirX, bulletStatePacket.dirY);

    return true;
}

