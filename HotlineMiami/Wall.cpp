#include "pch.h"
#include "Wall.h"

#include <algorithm>
#include <cmath>
#include <cfloat>

// ===================== 리소스 로드 =====================
void Wall::LoadImages(ImageManager& imgMgr)
{
    imgManager = &imgMgr;

    imgMgr.LoadSpriteImage(L"Resource/Map/sprWallBrickH.png", L"sprWallBrickH");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprWallBrickV.png", L"sprWallBrickV");

    // 사전 등록: 실제 크기와 충돌 형태 지정
    dict[BrickH] = {
        L"sprWallBrickH",
        Gdiplus::Rect(0, 0, 32, 8),
        true, true,
        CollisionKind::StripH, 8, Align::Center
    };

    dict[BrickV] = {
        L"sprWallBrickV",
        Gdiplus::Rect(0, 0, 8, 32),
        true, true,
        CollisionKind::StripV, 8, Align::Center
    };
}

void Wall::Init()
{
    instances.clear();
    for (int y = 0; y < ROWS; ++y)
        for (int x = 0; x < COLS; ++x) {
            grid[y][x] = None;
            colliderLUT[y][x].clear();
        }
    colliders.clear();
    cache.reset();
    cacheDirty = true;
}

void Wall::ClearAll()
{
    instances.clear();
    cacheDirty = true;
}

void Wall::AddWall(uint8_t id, int gx, int gy, int wTiles, int hTiles, DrawMode mode)
{
    if (wTiles <= 0 || hTiles <= 0) return;
    instances.push_back({ id, gx, gy, wTiles, hTiles, mode });
    cacheDirty = true;
}

POINT Wall::WorldToGrid(const Gdiplus::PointF& p)
{
    int gx = static_cast<int>((p.X - START_X) / TILE_W);
    int gy = static_cast<int>((p.Y - START_Y) / TILE_H);
    return { gx, gy };
}

Gdiplus::RectF Wall::GridToWorldRect(int gx, int gy)
{
    return {
        static_cast<float>(START_X + gx * TILE_W),
        static_cast<float>(START_Y + gy * TILE_H),
        static_cast<float>(TILE_W),
        static_cast<float>(TILE_H)
    };
}

// 점유맵 + 콜라이더
void Wall::RebuildOccupancyOnly()
{
    for (int y = 0; y < ROWS; ++y)
        for (int x = 0; x < COLS; ++x) {
            grid[y][x] = None;
            colliderLUT[y][x].clear();
        }
    colliders.clear();

    for (const auto& inst : instances) {
        const auto it = dict.find(inst.id);
        if (it == dict.end()) continue;
        const auto& d = it->second;

        int x0 = std::max(0, inst.gx);
        int y0 = std::max(0, inst.gy);
        int x1 = std::min(COLS - 1, inst.gx + inst.wTiles - 1);
        int y1 = std::min(ROWS - 1, inst.gy + inst.hTiles - 1);

        for (int gy = y0; gy <= y1; ++gy) {
            for (int gx = x0; gx <= x1; ++gx) {
                grid[gy][gx] = inst.id;

                Gdiplus::RectF tile = GridToWorldRect(gx, gy);
                Gdiplus::RectF rc = tile;

                if (d.ck == CollisionKind::StripH) {
                    rc.Height = static_cast<float>(std::min(d.thicknessPx, TILE_H));
                    switch (d.align) {
                    case Align::Start:  rc.Y = tile.Y; break;
                    case Align::Center: rc.Y = tile.Y + (TILE_H - rc.Height) * 0.5f; break;
                    case Align::End:    rc.Y = tile.GetBottom() - rc.Height; break;
                    }
                }
                else if (d.ck == CollisionKind::StripV) {
                    rc.Width = static_cast<float>(std::min(d.thicknessPx, TILE_W));
                    switch (d.align) {
                    case Align::Start:  rc.X = tile.X; break;
                    case Align::Center: rc.X = tile.X + (TILE_W - rc.Width) * 0.5f; break;
                    case Align::End:    rc.X = tile.GetRight() - rc.Width; break;
                    }
                }

                int idx = (int)colliders.size();
                colliders.push_back({ rc, d.blocksMove, d.blocksProjectile });
                colliderLUT[gy][gx].push_back(idx);
            }
        }
    }
}

// 렌더 캐시 
void Wall::RebuildCache()
{
    if (!imgManager) return;
    RebuildOccupancyOnly();

    const int W = COLS * TILE_W;
    const int H = ROWS * TILE_H;
    cache = std::make_unique<Gdiplus::Bitmap>(W, H, PixelFormat32bppARGB);

    Gdiplus::Graphics gg(cache.get());
    gg.SetSmoothingMode(Gdiplus::SmoothingModeNone);
    gg.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
    gg.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
    gg.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
    gg.Clear(Gdiplus::Color(0, 0, 0, 0));

    for (const auto& inst : instances) {
        auto it = dict.find(inst.id);
        if (it == dict.end()) continue;

        const Desc& d = it->second;
        Gdiplus::Image* img = imgManager->GetImage(d.imageKey);
        if (!img) continue;

        // 인스턴스 영역의 타일 범위를 직접 돈다 (타일당 1회만 그림)
        for (int ty = 0; ty < inst.hTiles; ++ty) {
            for (int tx = 0; tx < inst.wTiles; ++tx) {
                const int gx = inst.gx + tx;
                const int gy = inst.gy + ty;

                // 캐시 좌표계 기준 타일 사각형
                Gdiplus::Rect tileDst(gx * TILE_W, gy * TILE_H, TILE_W, TILE_H);

                if (d.ck == CollisionKind::FullTile) {
                    // 한 타일을 딱 1번 그림 
                    gg.DrawImage(img, tileDst, d.src.X, d.src.Y, d.src.Width, d.src.Height, Gdiplus::UnitPixel);
                }
                else if (d.ck == CollisionKind::StripH) {
                    // 수평 띠: 높이 = thicknessPx, 위치 = Align (위/중앙/아래)
                    int h = std::min(d.thicknessPx, TILE_H);
                    int y = tileDst.Y;
                    switch (d.align) {
                    case Align::Start:  y = tileDst.Y; break; // 위 붙임
                    case Align::Center: y = tileDst.Y + (TILE_H - h) / 2; break;
                    case Align::End:    y = tileDst.Y + TILE_H - h; break; // 아래 붙임
                    }
                    Gdiplus::Rect stripDst(tileDst.X, y, TILE_W, h);
                    gg.DrawImage(img, stripDst, d.src.X, d.src.Y, d.src.Width, d.src.Height, Gdiplus::UnitPixel);
                }
                else if (d.ck == CollisionKind::StripV) {
                    // 수직 띠: 너비 = thicknessPx, 위치 = Align (좌/중앙/우)
                    int w = std::min(d.thicknessPx, TILE_W);
                    int x = tileDst.X;
                    switch (d.align) {
                    case Align::Start:  x = tileDst.X; break; // 왼쪽 붙임
                    case Align::Center: x = tileDst.X + (TILE_W - w) / 2; break;
                    case Align::End:    x = tileDst.X + TILE_W - w; break; // 오른쪽 붙임
                    }
                    Gdiplus::Rect stripDst(x, tileDst.Y, w, TILE_H);
                    gg.DrawImage(img, stripDst, d.src.X, d.src.Y, d.src.Width, d.src.Height, Gdiplus::UnitPixel);
                }
            }
        }
    }

    cacheDirty = false;
}

// 렌더 
void Wall::Render(Gdiplus::Graphics& g)
{
    if (cacheDirty) RebuildCache();
    if (cache) g.DrawImage(cache.get(), START_X, START_Y);
}

bool Wall::ResolveMove(Gdiplus::PointF& pos, Gdiplus::SizeF aabb, Gdiplus::PointF delta) const
{
    if (delta.X == 0.0f && delta.Y == 0.0f)
        return false;

    // 이동 후의 AABB
    Gdiplus::RectF movedRect(
        pos.X,
        pos.Y,
        aabb.Width,
        aabb.Height
    );

    bool blocked = false;

    // 모든 collider 검사
    for (const auto& col : colliders)
    {
        if (!col.blocksMove)
            continue;

        if (!movedRect.IntersectsWith(col.rect))
            continue;

        blocked = true;

        // 충돌한 방향으로 되돌리기
        if (fabs(delta.X) > fabs(delta.Y))
        {
            // X축 보정
            if (delta.X > 0)
                pos.X = col.rect.X - aabb.Width;         // 오른쪽 이동  왼쪽으로 밀기
            else
                pos.X = col.rect.GetRight();             // 왼쪽 이동  오른쪽으로 밀기
        }
        else
        {
            // Y축 보정
            if (delta.Y > 0)
                pos.Y = col.rect.Y - aabb.Height;        // 아래 이동  위로 밀기
            else
                pos.Y = col.rect.GetBottom();            // 위 이동  아래로 밀기
        }

        // 보정 후 다시 movedRect 업데이트
        movedRect.X = pos.X;
        movedRect.Y = pos.Y;
    }

    return blocked;
}
