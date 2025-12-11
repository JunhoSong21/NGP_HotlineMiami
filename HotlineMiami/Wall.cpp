#include "pch.h"
#include "Wall.h"

#include <algorithm>
#include <cmath>
#include <cfloat>

// ===================== 리소스 로드 =====================
void Wall::LoadImages(ImageManager& imgMgr)
{
    imgManager = &imgMgr;
    // 벽 종류
    imgMgr.LoadSpriteImage(L"Resource/Map/sprWallBrickH.png", L"sprWallBrickH");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprWallBrickV.png", L"sprWallBrickV");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprWallHeavyH.png", L"sprWallHeavyH");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprWallHeavyV.png", L"sprWallHeavyV");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprCellWallH.png", L"sprCellWallH");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprCellWallV.png", L"sprCellWallV");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprSewerWallH.png", L"sprSewerWallH");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprSewerWallV.png", L"sprSewerWallV");
    // 엄폐물(가구) 종류
    imgMgr.LoadSpriteImage(L"Resource/Map/sprBarBooth.png", L"sprBarBooth");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprPoolTable.png", L"sprPoolTable");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprBigBed.png", L"sprBigBed");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprBossSofa.png", L"sprBossSofa");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprPoolTable2.png", L"sprPoolTable2");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprDJTable.png", L"sprDJTable");
    imgMgr.LoadSpriteImage(L"Resource/Map/sprHospitalCouch.png", L"sprHospitalCouch");

    // 사전 등록: 실제 크기와 충돌 형태 지정
    dict[BrickH] = {
        L"sprWallBrickH",
        Gdiplus::Rect(0, 0, 32, 8),
        true, true,
        CollisionKind::StripH, 8, Align::Start
    };

    dict[BrickV] = {
        L"sprWallBrickV",
        Gdiplus::Rect(0, 0, 8, 32),
        true, true,
        CollisionKind::StripV, 8, Align::Start
    };

    dict[HeavyH] = {
        L"sprWallHeavyH",
        Gdiplus::Rect(0, 0, 32, 8),
        true, true,
        CollisionKind::StripH, 8, Align::Start
    };

    dict[HeavyV] = {
        L"sprWallHeavyV",
        Gdiplus::Rect(0, 0, 8, 32),
        true, true,
        CollisionKind::StripV, 8, Align::End
    };

    dict[CellH] = {
    L"sprCellWallH",
    Gdiplus::Rect(0, 0, 32, 8),
    true, true,
    CollisionKind::StripH, 8, Align::Start
    };

    dict[CellV] = {
        L"sprCellWallV",
        Gdiplus::Rect(0, 0, 8, 32),
        true, true,
        CollisionKind::StripV, 8, Align::Start
    };

    dict[SewerH] = {
        L"sprSewerWallH",
        Gdiplus::Rect(0, 0, 32, 8),
        true, true,
        CollisionKind::StripH, 8, Align::Start
    };

    dict[SewerV] = {
        L"sprSewerWallV",
        Gdiplus::Rect(0, 0, 8, 32),
        true, true,
        CollisionKind::StripV, 8, Align::Start
    };

    dict[BarBooth] = {
     L"sprBarBooth",
     Gdiplus::Rect(0, 0, 72, 64),
     true, true,
     CollisionKind::StripH, 0, Align::Start
    };

     dict[PoolTable] = {
     L"sprPoolTable",
     Gdiplus::Rect(0, 0, 32, 68),
     true, true,
     CollisionKind::StripV, 0, Align::Start
    };

     dict[BigBed] = {
     L"sprBigBed",
     Gdiplus::Rect(0, 0, 60, 80),
     true, true,
     CollisionKind::StripV, 0, Align::Start
    };

     dict[BossSofa] = {
     L"sprBossSofa",
     Gdiplus::Rect(0, 0, 128, 64),
     true, true,
     CollisionKind::StripH, 0, Align::Start
     };


     dict[PoolTable2] = {
     L"sprPoolTable2",
     Gdiplus::Rect(0, 0, 68, 32),
     true, true,
     CollisionKind::StripH, 0, Align::Start
     };

     dict[DJTable] = {
     L"sprDJTable",
     Gdiplus::Rect(0, 0, 88, 23),
     true, true,
     CollisionKind::StripH, 0, Align::Start
     };

     dict[HospitalCouch] = {
     L"sprHospitalCouch",
     Gdiplus::Rect(0, 0, 25, 64),
     true, true,
     CollisionKind::StripV, 0, Align::Start
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
        const Desc& d = it->second;

        int x0 = std::max(0, inst.gx);
        int y0 = std::max(0, inst.gy);
        int x1 = std::min(COLS - 1, inst.gx + inst.wTiles - 1);
        int y1 = std::min(ROWS - 1, inst.gy + inst.hTiles - 1);
        if (inst.drawMode == DrawMode::Sprite)
        {
            // 월드 좌표계 기준
            const float baseX = static_cast<float>(START_X + inst.gx * TILE_W);
            const float baseY = static_cast<float>(START_Y + inst.gy * TILE_H);
            const float regionW = static_cast<float>(inst.wTiles * TILE_W);
            const float regionH = static_cast<float>(inst.hTiles * TILE_H);

            const float rcX = baseX + (regionW - d.src.Width) * 0.5f;
            const float rcY = baseY + regionH - d.src.Height;

            Gdiplus::RectF rc(
                rcX,
                rcY,
                static_cast<float>(d.src.Width),
                static_cast<float>(d.src.Height)
            );

            // 콜라이더 등록
            const int idx = static_cast<int>(colliders.size());
            colliders.push_back({ rc, d.blocksMove, d.blocksProjectile });

            // 이 콜라이더가 겹치는 타일들에 LUT 등록 + grid 표시
            const float left = rc.X - START_X;
            const float top = rc.Y - START_Y;
            const float right = rc.GetRight() - START_X;
            const float bottom = rc.GetBottom() - START_Y;

            int gx0 = static_cast<int>(std::floor(left / TILE_W));
            int gy0 = static_cast<int>(std::floor(top / TILE_H));
            int gx1 = static_cast<int>(std::floor((right - 0.001f) / TILE_W));
            int gy1 = static_cast<int>(std::floor((bottom - 0.001f) / TILE_H));

            gx0 = std::max(0, gx0);
            gy0 = std::max(0, gy0);
            gx1 = std::min(COLS - 1, gx1);
            gy1 = std::min(ROWS - 1, gy1);

            for (int gy = gy0; gy <= gy1; ++gy) {
                for (int gx = gx0; gx <= gx1; ++gx) {
                    grid[gy][gx] = inst.id;
                    colliderLUT[gy][gx].push_back(idx);
                }
            }
            continue;
        }

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

        // 1) Sprite 모드: 스프라이트 전체를 한 번에 그림
        if (inst.drawMode == DrawMode::Sprite)
        {
            // 캐시 좌표계 기준 (0,0)부터 시작, START_X/START_Y 없음
            const float baseX = static_cast<float>(inst.gx * TILE_W);
            const float baseY = static_cast<float>(inst.gy * TILE_H);
            const float regionW = static_cast<float>(inst.wTiles * TILE_W);
            const float regionH = static_cast<float>(inst.hTiles * TILE_H);

            // 타일 w×h 영역 안에서 중앙 정렬 + 바닥 맞춤
            const float dstX = baseX + (regionW - d.src.Width) * 0.5f;
            const float dstY = baseY + regionH - d.src.Height;

            Gdiplus::RectF dst(
                dstX,
                dstY,
                static_cast<float>(d.src.Width),
                static_cast<float>(d.src.Height)
            );

            gg.DrawImage(img, dst,
                d.src.X, d.src.Y, d.src.Width, d.src.Height,
                Gdiplus::UnitPixel);

            // Sprite 모드는 여기서 끝 (타일 루프 X)
            continue;
        }

        // 2) 나머지(Tiled / Stretch)는 기존 타일 기반 루프 사용
        for (int ty = 0; ty < inst.hTiles; ++ty) {
            for (int tx = 0; tx < inst.wTiles; ++tx) {
                const int gx = inst.gx + tx;
                const int gy = inst.gy + ty;

                Gdiplus::Rect tileDst(gx * TILE_W, gy * TILE_H, TILE_W, TILE_H);

                if (d.ck == CollisionKind::FullTile) {
                    gg.DrawImage(img, tileDst,
                        d.src.X, d.src.Y, d.src.Width, d.src.Height,
                        Gdiplus::UnitPixel);
                }
                else if (d.ck == CollisionKind::StripH) {
                    int h = std::min(d.thicknessPx, TILE_H);
                    int y = tileDst.Y;
                    switch (d.align) {
                    case Align::Start:  y = tileDst.Y; break;
                    case Align::Center: y = tileDst.Y + (TILE_H - h) / 2; break;
                    case Align::End:    y = tileDst.Y + TILE_H - h; break;
                    }
                    Gdiplus::Rect stripDst(tileDst.X, y, TILE_W, h);
                    gg.DrawImage(img, stripDst,
                        d.src.X, d.src.Y, d.src.Width, d.src.Height,
                        Gdiplus::UnitPixel);
                }
                else if (d.ck == CollisionKind::StripV) {
                    int w = std::min(d.thicknessPx, TILE_W);
                    int x = tileDst.X;
                    switch (d.align) {
                    case Align::Start:  x = tileDst.X; break;
                    case Align::Center: x = tileDst.X + (TILE_W - w) / 2; break;
                    case Align::End:    x = tileDst.X + TILE_W - w; break;
                    }
                    Gdiplus::Rect stripDst(x, tileDst.Y, w, TILE_H);
                    gg.DrawImage(img, stripDst,
                        d.src.X, d.src.Y, d.src.Width, d.src.Height,
                        Gdiplus::UnitPixel);
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
    // 이동 없으면 처리할 것 없음
    if ((delta.X == 0.0f && delta.Y == 0.0f) || colliders.empty())
        return false;

    // pos 는 "플레이어 중심 좌표"
    const float halfW = aabb.Width * 0.5f;
    const float halfH = aabb.Height * 0.5f;

    // 한 프레임 이동 길이
    float maxLen = std::max(std::fabs(delta.X), std::fabs(delta.Y));
    if (maxLen <= 0.0f)
        return false;

    // 한 스텝 최대 이동량 (타일의 1/8 정도)
    const float maxStep = 4.0f; // TILE_W(32) 기준

    int steps = static_cast<int>(std::ceil(maxLen / maxStep));
    if (steps < 1) steps = 1;

    Gdiplus::PointF step(delta.X / steps, delta.Y / steps);

    auto intersectsAt = [&](const Gdiplus::PointF& centerPos) -> bool
        {
            Gdiplus::RectF rect(
                centerPos.X - halfW,
                centerPos.Y - halfH,
                aabb.Width,
                aabb.Height
            );

            for (const auto& col : colliders)
            {
                if (!col.blocksMove)
                    continue;
                if (rect.IntersectsWith(col.rect))
                    return true;
            }
            return false;
        };

    bool blocked = false;

    // 여러 작은 스텝으로 나누어 이동
    for (int i = 0; i < steps; ++i)
    {
        // 1) 전체 (X+Y) 이동 시도
        Gdiplus::PointF tryFull(pos.X + step.X, pos.Y + step.Y);
        if (!intersectsAt(tryFull))
        {
            pos = tryFull;
            continue;
        }

        // 2) 전체가 막히면 X만 이동 시도 (슬라이딩)
        bool movedThisStep = false;
        if (step.X != 0.0f)
        {
            Gdiplus::PointF tryX(pos.X + step.X, pos.Y);
            if (!intersectsAt(tryX))
            {
                pos = tryX;
                movedThisStep = true;
                // Y축 이동은 이 스텝에서 막힌 것으로 보고 다음 스텝에서도 0 유지
                step.Y = 0.0f;
            }
        }

        // 3) X도 막혔다면 Y만 이동 시도
        if (!movedThisStep && step.Y != 0.0f)
        {
            Gdiplus::PointF tryY(pos.X, pos.Y + step.Y);
            if (!intersectsAt(tryY))
            {
                pos = tryY;
                movedThisStep = true;
                step.X = 0.0f;
            }
        }

        // 4) 둘 다 막혀 있으면 이 방향 이동은 더 이상 불가
        if (!movedThisStep)
        {
            blocked = true;
            break;
        }
    }

    return blocked;
}

bool Wall::Raycast(const Gdiplus::PointF& start,
    const Gdiplus::PointF& dir,
    float distance,
    Gdiplus::PointF* hitPos,
    POINT* hitCell) const
{
    // 이동 거리가 0 이하면 충돌 없음
    if (distance <= 0.0f)
        return false;

    // 방향 벡터 길이 검사 및 정규화
    float len = std::sqrt(dir.X * dir.X + dir.Y * dir.Y);
    if (len <= 0.0001f)
        return false;

    Gdiplus::PointF nDir(dir.X / len, dir.Y / len);

    // ResolveMove 에 맞춰서 step = 4.0f 사용
    const float maxStep = 4.0f;
    float travelled = 0.0f;

    while (travelled <= distance)
    {
        // 현재 샘플링 위치
        Gdiplus::PointF p(
            start.X + nDir.X * travelled,
            start.Y + nDir.Y * travelled
        );
        // 월드 → 그리드 좌표
        POINT cell = WorldToGrid(p);

        // 그리드 범위 밖이면 그냥 다음 위치로 진행
        if (cell.x >= 0 && cell.x < COLS &&
            cell.y >= 0 && cell.y < ROWS)
        {
            // grid[y][x] == None 이면 빈칸, 아니면 벽
            if (grid[cell.y][cell.x] != None)
            {
                if (hitPos)
                    *hitPos = p;

                if (hitCell) {
                    hitCell->x = cell.x;
                    hitCell->y = cell.y;
                }
               return true;
            }
        }
        travelled += maxStep;
    }
    return false;
}
