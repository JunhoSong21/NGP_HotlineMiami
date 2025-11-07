#pragma once
#include "pch.h"
#include "ImageManager.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <memory>

class Wall final {
public:
    static constexpr int COLS = 30;
    static constexpr int ROWS = 20;
    static constexpr int TILE_W = 32;
    static constexpr int TILE_H = 32;
    static constexpr int START_X = 50;
    static constexpr int START_Y = 50;

    enum : uint8_t {
        None = 0,
        BrickH = 1,   // 32x8  Resource/Map/sprWallBrickH.png
        BrickV = 2    // 8x32  Resource/Map/sprWallBrickV.png
    };

    enum class DrawMode : uint8_t { Tiled, Stretch };

    enum class CollisionKind : uint8_t { FullTile, StripH, StripV };
    enum class Align : uint8_t { Start, Center, End };

    struct Desc {
        std::wstring imageKey;
        Gdiplus::Rect src;
        bool blocksMove;
        bool blocksProjectile;
        CollisionKind ck;
        int thicknessPx;
        Align align;
    };

    struct Instance {
        uint8_t id;
        int gx, gy;
        int wTiles, hTiles;
        DrawMode drawMode;
    };

    struct Collider {
        Gdiplus::RectF rect;
        bool blocksMove;
        bool blocksProjectile;
    };

public:
    Wall() = default;
    ~Wall() = default;

    void LoadImages(ImageManager& imgMgr);
    void Init();

    void AddWall(uint8_t id, int gx, int gy, int wTiles, int hTiles, DrawMode mode = DrawMode::Tiled);
    void ClearAll();

    void Render(Gdiplus::Graphics& g);

    bool ResolveMove(Gdiplus::PointF& pos, Gdiplus::SizeF aabbSize, Gdiplus::PointF delta) const;
    bool Raycast(const Gdiplus::PointF& origin, const Gdiplus::PointF& dir, float maxDist,
        Gdiplus::PointF* hitPoint = nullptr, POINT* hitCell = nullptr) const;

    static POINT WorldToGrid(const Gdiplus::PointF& p);
    static Gdiplus::RectF GridToWorldRect(int gx, int gy);

private:
    bool IsInside(int gx, int gy) const { return (gx >= 0 && gy >= 0 && gx < COLS && gy < ROWS); }

    void RebuildCache();
    void RebuildOccupancyOnly();

private:
    ImageManager* imgManager{ nullptr };

    std::map<uint8_t, Desc> dict;
    std::vector<Instance> instances;

    uint8_t grid[ROWS][COLS]{};
    std::vector<Collider> colliders;
    std::vector<int> colliderLUT[ROWS][COLS];

    std::unique_ptr<Gdiplus::Bitmap> cache;
    bool cacheDirty{ true };
};
