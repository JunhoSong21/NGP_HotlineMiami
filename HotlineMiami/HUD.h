#pragma once
#include "pch.h"
#include "ImageManager.h"

class HUD
{
private:
    // --- Aim Cursor ----
    std::wstring aimSpriteKey;
    float aimWidth;
    float aimHeight;

public:
    HUD();
    ~HUD() = default;

    // 초기화 (이미지 로드)
    void Init(ImageManager& imgMgr);

    // 전체 HUD 렌더
    void Render(HWND hWnd, Gdiplus::Graphics& g, ImageManager& imgMgr);

private:
    // 개별 요소 렌더
    void RenderAimCursor(HWND hWnd, Gdiplus::Graphics& g, ImageManager& imgMgr);
};
