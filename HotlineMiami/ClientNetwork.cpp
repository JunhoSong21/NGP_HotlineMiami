#include "pch.h"
#include "ClientNetwork.h"
#include "NetProtocol.h"
#include "Player.h"

int Send_Input(SOCKET sock, HWND hWnd, const Player& player)
{
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

    // 4) 패킷을 char 버퍼로 복사
    char buffer[sizeof(pkt)];
    std::memcpy(buffer, &pkt, sizeof(pkt));

    // 5) 서버로 전송
    int retval = send(sock, buffer, sizeof(buffer), 0);

    return retval;

}