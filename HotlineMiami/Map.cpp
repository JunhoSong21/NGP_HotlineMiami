#pragma once
#include "pch.h"
#include "Map.h"

using namespace Gdiplus;

void Map::LoadMapImages(ImageManager& imgMgr)
{
    imgManager = &imgMgr;
    // 필요한 타일 아틀라스 로드
    imgManager->LoadSpriteImage(L"Resource/Map/Tiles.png", L"tiles1");
    tilesKey = L"tiles1";
}

void Map::Init()
{
    // 테스트: 전부 Tile1로 채움 (원하면 원하는 값으로 채운 뒤 cacheDirty = true)
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            map[y][x] = Tile1;

    cacheDirty = true;
}

void Map::Render(Graphics& g)
{
    if (cacheDirty || !groundCache)
        RebuildCache();

    const int w = cols * tileW;
    const int h = rows * tileH;

    // 맵은 화면에 완전 덮어쓰기 (잔상 방지)
    auto s = g.Save();
    g.SetCompositingMode(CompositingModeSourceCopy);
    g.DrawImage(groundCache.get(), Rect(startX, startY, w, h));
    g.Restore(s);
}

void Map::RebuildCache()
{
    if (!imgManager) return;

    Bitmap* tiles = imgManager->GetImage(tilesKey);
    if (!tiles) return;

    const int w = cols * tileW;
    const int h = rows * tileH;

    groundCache = std::make_unique<Bitmap>(w, h, PixelFormat32bppARGB);

    Graphics gg(groundCache.get());
    gg.SetSmoothingMode(SmoothingModeNone);
    gg.SetInterpolationMode(InterpolationModeNearestNeighbor);
    gg.SetCompositingQuality(CompositingQualityHighSpeed);
    gg.SetCompositingMode(CompositingModeSourceCopy);
    gg.Clear(Color(255, 0, 0, 0)); // (불투명) 초기화

    const Rect src(0, 0, 16, 16);

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if (map[y][x] != Tile1) continue;

            const Rect dst(x * tileW, y * tileH, tileW, tileH);
            gg.DrawImage(tiles, dst, src.X, src.Y, src.Width, src.Height, UnitPixel);
        }
    }

    cacheDirty = false;
}
