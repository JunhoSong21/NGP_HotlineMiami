// Map.h
#pragma once
#include "pch.h"
#include "ImageManager.h"
#include <memory>
#include <cstdint>

class Map {
public:
    Map() = default;

    // 타일 종류 (렌더 전용)
    enum : uint8_t { Blank = 0, Tile1 = 1 };
    void LoadMapImages(ImageManager& imgMgr);
    void Init();
    void Render(Gdiplus::Graphics& g);

private:
    void RebuildCache(); // 타일 전체를 그려둔 캐시 비트맵 재생성

private:
    ImageManager* imgManager{ nullptr };
    std::wstring         tilesKey;

    static const int cols = 40;
    static const int rows = 30;
    int tileW = 32;  // 출력 크기
    int tileH = 32;  // 출력 크기
    int startX = 50;  // 화면 시작 위치
    int startY = 50;  // 화면 시작 위치

    uint8_t map[rows][cols]{}; // 0: Blank, 1: Tile1

    // 잔상 방지
    std::unique_ptr<Gdiplus::Bitmap> groundCache;
    bool cacheDirty{ true };
};
