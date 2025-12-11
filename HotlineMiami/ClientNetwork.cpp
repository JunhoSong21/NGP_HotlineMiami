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
    HWND     hWnd       = p->hWnd;
    Player** players    = p->players;
    Bullet** bullets    = p->bullets;
    Grenade** grenades  = p->grenades;

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
        
        RecvProcess(g_ClientSock, players, bullets, grenades);

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

    g_MyPlayerIndex = loginSuccess.targetNum;
}

// Send 구조
int SendProcess(SOCKET sock, HWND hWnd, const Player& player)
{
    // 총알 요청이 있으면 총알부터
    if (g_BulletReq.requested) {
        if (Send_BulletTrigger(sock, g_BulletReq.dirRadAngle) == SOCKET_ERROR) {
            return SOCKET_ERROR;
		}
		g_BulletReq.requested = false;

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

int Send_BulletTrigger(SOCKET sock, float dirRadAngle)
{
    CS_BULLET_TRIGGER pkt{};
    pkt.dirRadAngle = dirRadAngle;
    PacketHeader header{};
    header.packetType = PN::CS_BULLET_TRIGGER;
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
void RecvProcess(SOCKET sock, Player** players, Bullet** bullets, Grenade** grenades)
{
    PacketHeader header{};
    int retValue = recv(sock, (char*)&header, sizeof(header), MSG_WAITALL);
    if (retValue == SOCKET_ERROR)
        return;

    switch (header.packetType) {
    case PN::SC_PLAYER_MOVE: {
        Recv_PlayerMove(players);
        break;
    }
    case PN::SC_BULLET_STATE: {
        Recv_BulletData(bullets);
        break;
    }
    case PN::SC_GRENADE_STATE: {
        Recv_GrenadeData(grenades);
        break;
    }
    case PN::SC_GAME_END: {
        Recv_GameEnd();
        break;
    }
    default:
        // Undefined Event
        break;
    }
}

void Recv_PlayerMove(Player** players)
{
    int retValue = 0;
    SC_PLAYER_MOVE playerMovePacket{};

    for (int i = 0; i < MAX_CLIENT_NUM; ++i) {
        retValue = recv(g_ClientSock, (char*)&playerMovePacket, sizeof(playerMovePacket), MSG_WAITALL);
        if (retValue == SOCKET_ERROR)
            return;

        int index = static_cast<int>(playerMovePacket.targetNum);
        if (!players[index])
            return;

        players[index]->SetStatus(playerMovePacket.isAlive, playerMovePacket.hp);
        if (index == g_MyPlayerIndex) {
            players[index]->SetPosition(
                playerMovePacket.posX,
                playerMovePacket.posY
            );
        }
        else {
            players[index]->SetPositionAndAngle(
                playerMovePacket.posX,
                playerMovePacket.posY,
                playerMovePacket.angle
            );
        }
    }
}

void Recv_BulletData(Bullet** bullets)
{
    int retValue = 0;
    SC_BULLET_STATE bulletStatePacket{};

    for (int i = 0; i < MAX_CLIENT_NUM; ++i) {
        retValue = recv(g_ClientSock, (char*)&bulletStatePacket, sizeof(bulletStatePacket), MSG_WAITALL);
        if (retValue == SOCKET_ERROR)
            return;

        int index = static_cast<int>(bulletStatePacket.targerNum);
        if (!bullets[index])
            return;

        bullets[index]->SetVisible(bulletStatePacket.isActive);
        bullets[index]->SetPosition(bulletStatePacket.posX, bulletStatePacket.posY);
        bullets[index]->SetDirection(bulletStatePacket.dirAngle);
    }
}

void Recv_GrenadeData(Grenade** grenades)
{
    int retValue = 0;
    SC_GRENADE_STATE grenadeStatePacket{};

    for (int i = 0; i < MAX_CLIENT_NUM; ++i) {
        retValue = recv(g_ClientSock, (char*)&grenadeStatePacket, sizeof(grenadeStatePacket), MSG_WAITALL);
        if (retValue == SOCKET_ERROR)
            return;

        int index = static_cast<int>(grenadeStatePacket.targetNum);
        if (!grenades[index])
            return;

        grenades[index]->SetIsActive(grenadeStatePacket.isActive);
        grenades[index]->SetIsExplode(grenadeStatePacket.isExploded);
        grenades[index]->SetPosition(grenadeStatePacket.posX, grenadeStatePacket.posY);
    }
}

void Recv_GameEnd()
{
    g_NetworkRunning = false;
}