// HUD.cpp
#include "pch.h"
#include "HUD.h"

HUD::HUD()
    : aimSpriteKey(L"Aim"),
    aimWidth(11.0f),
    aimHeight(11.0f)
{
}

void HUD::Init(ImageManager& imgMgr)
{
    // 조준점 이미지 로드
    imgMgr.LoadSpriteImage(L"Resource/Sprite/Aim.png", aimSpriteKey);
}

void HUD::Render(HWND hWnd, Gdiplus::Graphics& g, ImageManager& imgMgr)
{
    // --- HUD 구성 요소 렌더 ---
    RenderAimCursor(hWnd, g, imgMgr);

    // ※ 앞으로 HP바, 탄창, 스킬 쿨타임, 미니맵 등 추가할 때
    //    여기 함수에 호출을 추가하면 된다.
}

void HUD::RenderAimCursor(HWND hWnd, Gdiplus::Graphics& g, ImageManager& imgMgr)
{
    Gdiplus::Bitmap* bmp = imgMgr.GetImage(aimSpriteKey);
    if (!bmp) return;

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);

    Gdiplus::RectF dst(
        pt.x - aimWidth * 0.5f,
        pt.y - aimHeight * 0.5f,
        aimWidth,
        aimHeight
    );

    auto s = g.Save();
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

    g.DrawImage(
        bmp,
        dst,
        0, 0,
        aimWidth, aimHeight,
        Gdiplus::UnitPixel
    );

    g.Restore(s);
}
