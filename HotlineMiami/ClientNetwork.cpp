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
LoginRequest g_LoginReq = { false, "" };
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

    // CS_LOGIN_PACKET
    if (g_LoginReq.requested)
    {
        CS_LOGIN_PACKET lpkt{};
        strncpy_s(lpkt.clientIp, g_LoginReq.ip, sizeof(lpkt.clientIp) - 1);

        PacketHeader lheader{};
        lheader.packetType = PN::CS_LOGIN_PACKET;
        lheader.packetSize = sizeof(PacketHeader) + sizeof(lpkt);

        send(sock, (char*)&lheader, sizeof(lheader), 0);
        send(sock, (char*)&lpkt, sizeof(lpkt), 0);

        g_LoginReq.requested = false;
    }

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

void RecvProcess(SOCKET sock, Player** players)
{
    PacketHeader header{};
    OutputDebugStringA("[PlayerRecv] recv header START\n");
    int retValue = recv(sock, (char*)&header, sizeof(header), 0);
    OutputDebugStringA("[PlayerRecv] recv header END\n");
    if (retValue <= 0)
        return;

    if (header.packetSize < sizeof(PacketHeader))
        return;
    int bodySize = static_cast<int>(header.packetSize - sizeof(PacketHeader));

    switch (header.packetType) {
    case PN::SC_PLAYER_MOVE: {
        if (bodySize != sizeof(SC_PLAYER_MOVE))
        {
            // 크기 안 맞으면 bodySize만큼 읽어서 버리기
            char dummy[256];
            int remain = bodySize;
            while (remain > 0)
            {
                int chunk = (remain > (int)sizeof(dummy)) ? (int)sizeof(dummy) : remain;
                int r = recv(sock, dummy, chunk, 0);
                if (r <= 0) return;
                remain -= r;
            }
            return;
        }

        SC_PLAYER_MOVE playerMovePacket{};
        retValue = recv(sock, (char*)&playerMovePacket, sizeof(playerMovePacket), MSG_WAITALL);
        if (retValue <= 0)
            return;

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

        break;
    }
    default:
    {
        // 아직 처리 안 하는 패킷은 bodySize만큼 읽어서 버리기
        char dummy[256];
        int remain = bodySize;
        while (remain > 0)
        {
            int chunk = (remain > (int)sizeof(dummy)) ? (int)sizeof(dummy) : remain;
            int r = recv(sock, dummy, chunk, 0);
            if (r <= 0) return;
            remain -= r;
        }
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
	Bullet* bullet = p->bullet;
    Player** players = p->players;

    OutputDebugStringA("[NET] Thread START\n");

    while (g_NetworkRunning)
    {
        // 매 프레임마다 최신 내 인덱스 가져오기
        int idx = g_MyPlayerIndex;

        // 아직 서버가 내 인덱스를 안 알려줬으면 일단 0번으로 가정해서 테스트 가능
        if (idx < 0 || idx >= 3)
            idx = 0;

        Player* myPlayer = players[idx];

        if (myPlayer)
        {
            if (Send_Input(g_ClientSock, hWnd, *myPlayer) == SOCKET_ERROR)
            {
                OutputDebugStringA("[NET] Send_Input failed -> g_NetworkRunning = false\n");
                g_NetworkRunning = false;
                break;
            }
        }
        
        // RecvProcess();
       if (RecvProcess(g_ClientSock, bullet) == -1)
        {
            OutputDebugStringA("[NET] Bullet Recv failed (returned -1) -> g_NetworkRunning = false\n");
            g_NetworkRunning = false;
            break;
        }

        RecvProcess(g_ClientSock, players);
        Sleep(16);
    }

    delete p; // 동적 할당했던 param delete
    OutputDebugStringA("[NET] Client_Network_Thread EXIT\n");
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

//int RecvProcess(SOCKET sock, Bullet* bullet)
//{
//    // 공통 헤더 먼저 받기
//    PacketHeader header{};
//    int ret = recv(sock, reinterpret_cast<char*>(&header),
//        sizeof(header), MSG_WAITALL);
//    
//    // debug
//    if (ret <= 0)
//    {
//        OutputDebugStringA("[BulletRecv] header recv <= 0, return -1\n");
//        return -1;  // 소켓 종료/에러
//    }
//
//    if (header.packetSize < sizeof(PacketHeader)) {
//        return 0;
//    }
//    int bodySize = static_cast<int>(header.packetSize - sizeof(PacketHeader));
//
//    // 2) 패킷 종류에 따라 분기
//    switch (header.packetType)
//    {
//    case PN::SC_BULLET_STATE: 
//    {
//        // 먼저 사이즈 체크
//        if (bodySize != sizeof(SC_BULLET_STATE)) {
//            // 크기 안 맞으면 이 패킷의 바디를 끝까지 읽어서 버퍼에서 제거
//            char dummy[256];
//            int remain = bodySize;
//            while (remain > 0) {
//                int chunk = (remain > (int)sizeof(dummy)) ? (int)sizeof(dummy) : remain;
//                int r = recv(sock, dummy, chunk, 0);
//                if (r <= 0) return -1;
//                remain -= r;
//            }
//            return 0;
//        }
//
//        // 크기가 정상이라면, 실제 총알 데이터 읽기
//        return Recv_BulletData(sock, bullet);
//    }
//    default: 
//    {
//        // bodySize만큼 dummy로 읽어서 버퍼에서 제거하고, 다음 패킷 헤더를 기다린다.
//        char dummy[256];
//        int remain = bodySize;
//        while (remain > 0) {
//            int chunk = (remain > (int)sizeof(dummy)) ? (int)sizeof(dummy) : remain;
//            int r = recv(sock, dummy, chunk, 0);
//            if (r <= 0) return -1;
//            remain -= r;
//        }
//        return 0;
//    }
//    }
//}

int RecvProcess(SOCKET sock, Bullet* bullet)
{
    // 1) 헤더 먼저 받기
    PacketHeader header{};
    OutputDebugStringA("[BulletRecv] recv header START\n");
    int ret = recv(sock, reinterpret_cast<char*>(&header),
        sizeof(header), MSG_WAITALL);
    OutputDebugStringA("[BulletRecv] recv header END\n");

    if (ret <= 0) {
        OutputDebugStringA("[BulletRecv] header recv <= 0, return -1\n");
        return -1;  // 서버가 끊었거나 에러
    }

    char dbg[128];
    sprintf_s(dbg, "[BulletRecv] header type=%d, size=%zu\n",
        header.packetType, header.packetSize);
    OutputDebugStringA(dbg);

    int bodySize = static_cast<int>(header.packetSize);
    if (bodySize < 0) {
        OutputDebugStringA("[BulletRecv] negative bodySize, ignore packet\n");
        return 0;
    }

    switch (header.packetType)
    {
    case PN::SC_BULLET_STATE:
    {
        // 크기 검사(선택)
        if (bodySize != sizeof(SC_BULLET_STATE)) {
            OutputDebugStringA("[BulletRecv] SC_BULLET_STATE size mismatch, consume dummy\n");
            // 잘못된 패킷 → bodySize만큼 dummy로 소비 후 무시
            char dummy[256];
            int remain = bodySize;
            while (remain > 0) {
                int chunk = (remain > (int)sizeof(dummy)) ? (int)sizeof(dummy) : remain;
                int r = recv(sock, dummy, chunk, 0);
                if (r <= 0) {
                    OutputDebugStringA("[BulletRecv] dummy recv <= 0 in BULLET_STATE, return -1\n");
                    return -1;
                }
                remain -= r;
            }
            return 0;
        }

        // 정상 크기 → 실제 데이터 읽기
        SC_BULLET_STATE pkt{};
        ret = recv(sock, reinterpret_cast<char*>(&pkt),
            sizeof(pkt), MSG_WAITALL);
        if (ret <= 0) {
            OutputDebugStringA("[BulletRecv] body recv <= 0 in BULLET_STATE, return -1\n");
            return -1;
        }

        if (!bullet) return 0;

        bullet->SetVisible(pkt.isActive != 0);
        bullet->SetPosition(pkt.posX, pkt.posY);
        // SC_BULLET_STATE가 dirAngle만 있는 구조라면 여기 맞춰서 수정 필요
        // bullet->SetDirection(pkt.dirX, pkt.dirY); // 이 부분은 struct 정의에 맞게 조정

        return 0;
    }

    default:
    {
        if (bodySize == 0) {
            // 바디가 없는 패킷이면 바로 리턴
            OutputDebugStringA("[BulletRecv] default packet with bodySize=0, nothing to consume\n");
            return 0;
        }

        OutputDebugStringA("[BulletRecv] default packet, consuming body as dummy\n");

        char dummy[256];
        int remain = bodySize;
        while (remain > 0) {
            int chunk = (remain > (int)sizeof(dummy)) ? (int)sizeof(dummy) : remain;
            int r = recv(sock, dummy, chunk, 0);
            if (r <= 0) {
                OutputDebugStringA("[BulletRecv] dummy recv <= 0 in default, return -1\n");
                return -1;
            }
            remain -= r;
        }
        return 0;
    }
    }
}
