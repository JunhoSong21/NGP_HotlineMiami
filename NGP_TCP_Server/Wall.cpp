#include "Wall.h"

void Wall::AddRect(int gx, int gy, int w, int h,
    bool blockMove, bool blockProjectile) {
    int maxX = std::min(gx + w, COLS);
    int maxY = std::min(gy + h, ROWS);

    for (int y = std::max(0, gy); y < maxY; ++y) {
        for (int x = std::max(0, gx); x < maxX; ++x) {
            Cell& c = At(x, y);
            c.blockMove = c.blockMove || blockMove;
            c.blockProjectile = c.blockProjectile || blockProjectile;
        }
    }
}

void Wall::InitDefaultLayout() {
    // 전부 초기화
    grid.fill(Cell{});

    // ======= 클라이언트 GameLoop::Init 에서 wall->AddWall(...) 한 것과 동일하게 =======
    // 내벽 - 플레이어 시작방 벽
    for (int i = 0; i < 10; ++i) {
        // wall->AddWall(Wall::HeavyH, i, 10, 1, 1, ...);
        AddRect(i, 10, 1, 1, true, true);
    }
    for (int i = 0; i < 8; ++i) {
        // wall->AddWall(Wall::HeavyV, 10, i, 1, 1, ...);
        AddRect(10, i, 1, 1, true, true);
    }
    for (int i = 0; i < 8; ++i) {
        // wall->AddWall(Wall::CellH, i, 20, 1, 1, ...);
        AddRect(i, 20, 1, 1, true, true);
    }
    for (int i = 0; i < 10; ++i) {
        // wall->AddWall(Wall::CellV, 10, 20 + i, 1, 1, ...);
        AddRect(10, 20 + i, 1, 1, true, true);
    }
    for (int i = 0; i < 5; ++i) {
        // wall->AddWall(Wall::SewerH, 33 + i, 15, 1, 1, ...);
        AddRect(33 + i, 15, 1, 1, true, true);
    }
    for (int i = 0; i < 15; ++i) {
        // wall->AddWall(Wall::SewerV, 33, i, 1, 1, ...);
        AddRect(33, i, 1, 1, true, true);
    }

    // 외벽
    for (int i = 0; i < 40; ++i) {
        // 위/아래
        AddRect(i, 0, 1, 1, true, true);  // BrickH
        AddRect(i, 30, 1, 1, true, true);
    }
    for (int i = 0; i < 30; ++i) {
        // 좌/우
        AddRect(0, i, 1, 1, true, true);  // BrickV
        AddRect(40, i, 1, 1, true, true);
    }

    // 엄폐물(가구) - 클라에서 DrawMode::Sprite 였던 것들
    // wTiles, hTiles 를 그대로 충돌 범위로 사용 (조금 넉넉한 AABB)
    AddRect(19, 8, 2, 1, true, true); // PoolTable2
    AddRect(19, 21, 2, 1, true, true);

    AddRect(20, 13, 1, 1, true, true); // BarBooth
    AddRect(19, 17, 3, 2, true, true); // BossSofa
    AddRect(16, 15, 1, 2, true, true); // HospitalCouch
    AddRect(6, 16, 2, 3, true, true); // PoolTable

    AddRect(9, 7, 1, 1, true, true); // BigBed x3
    AddRect(6, 22, 1, 1, true, true);
    AddRect(34, 14, 1, 1, true, true);

    AddRect(26, 14, 3, 1, true, true); // DJTable 2줄
    AddRect(26, 17, 3, 1, true, true);
}

bool Wall::IsProjectileBlocked(float x, float y) const {
    int gx, gy;
    if (!WorldToGrid(x, y, gx, gy))
        return true;  // 맵 밖이면 막힌 것으로 처리 (벽 밖으로 나가는 것 방지)

    return At(gx, gy).blockProjectile;
}

bool Wall::IsMoveBlocked(float cx, float cy, float radius) const {
    // 원 4방향 샘플링 (간단한 circle vs cell 충돌)
    const float dx[4] = { -radius,  radius,  0.0f,      0.0f };
    const float dy[4] = { 0.0f,    0.0f,   -radius,    radius };

    for (int i = 0; i < 4; ++i) {
        float px = cx + dx[i];
        float py = cy + dy[i];

        int gx, gy;
        if (WorldToGrid(px, py, gx, gy)) {
            if (At(gx, gy).blockMove)
                return true;
        }
        else {
            // 맵 밖도 벽 취급
            return true;
        }
    }
    return false;
}

void Wall::ResolveMove(float oldX, float oldY,
    float& newX, float& newY,
    float radius) const {
    // 1) 통째로 이동했을 때 벽과 안 겹치면 ok
    if (!IsMoveBlocked(newX, newY, radius)) {
        return;
    }

    // 2) X만 움직이고 Y는 되돌려보기
    float candX = newX;
    float candY = oldY;
    if (!IsMoveBlocked(candX, candY, radius)) {
        newX = candX;
        newY = candY;
        return;
    }

    // 3) Y만 움직이고 X는 되돌려보기
    candX = oldX;
    candY = newY;
    if (!IsMoveBlocked(candX, candY, radius)) {
        newX = candX;
        newY = candY;
        return;
    }

    // 4) 둘 다 안되면 원래 자리로 롤백
    newX = oldX;
    newY = oldY;
}
