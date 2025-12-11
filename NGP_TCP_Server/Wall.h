#pragma once
#include <array>
#include <algorithm>

class Wall {
public:
    static constexpr int COLS = 41;
    static constexpr int ROWS = 31;
    static constexpr int TILE_W = 32;
    static constexpr int TILE_H = 32;
    static constexpr int START_X = 50;
    static constexpr int START_Y = 50;

    struct Cell {
        bool blockMove = false;  // 플레이어 막기
        bool blockProjectile = false;  // 총알/수류탄/파편 막기
    };

    static Wall& Get() {
        static Wall inst;
        return inst;
    }

    // GameLoop::Init 에서 wall->AddWall(...) 했던 것과 동일한 배치를 서버에 세팅
    void InitDefaultLayout();

    // (cx, cy)를 중심으로 하는 원(플레이어 Hitbox)이 벽에 겹치는지
    bool IsMoveBlocked(float cx, float cy, float radius) const;

    // 탄/수류탄은 점 충돌로만 봐도 무방
    bool IsProjectileBlocked(float x, float y) const;

    // old → new 로 이동하려 할 때, 벽과 겹치면 axis 별로 밀어내서 slide
    void ResolveMove(float oldX, float oldY,
        float& newX, float& newY,
        float radius) const;

private:
    std::array<Cell, COLS* ROWS> grid{};

    bool WorldToGrid(float x, float y, int& gx, int& gy) const {
        gx = static_cast<int>((x - START_X) / TILE_W);
        gy = static_cast<int>((y - START_Y) / TILE_H);
        return (gx >= 0 && gy >= 0 && gx < COLS && gy < ROWS);
    }

    Cell& At(int gx, int gy) {
        return grid[gy * COLS + gx];
    }
    const Cell& At(int gx, int gy) const {
        return grid[gy * COLS + gx];
    }

    void AddRect(int gx, int gy, int w, int h,
        bool blockMove, bool blockProjectile);
};
