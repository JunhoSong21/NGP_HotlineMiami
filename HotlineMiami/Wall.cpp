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

// 렌더 캐시 
void Wall::RebuildCache()
{
    if (!imgManager) return;

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
