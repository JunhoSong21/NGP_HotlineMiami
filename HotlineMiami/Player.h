#pragma once
#include <cmath>
#include <numbers>
#include <string>
#include <map>

#include "ImageManager.h"
#include "SoundManager.h"

// Player State
enum PLAYER_STATE
{
	PLAYER_IDLE = 0,
	PLAYER_WALK,
	PLAYER_DEATH   // 사망 상태
};

class Player {
private:
	std::wstring playerState;
	Gdiplus::PointF playerPos;

	int			playerSpriteFrameNum;
	int			spriteOriginWidth;
	int			spriteOriginHeight;
	int			spriteScaleMag;
	float		vectorX;
	float		vectorY;
	float		playerSpeed;
	float		frameTimeAccumulate;
	float hp;      // 현재 체력 (0.0 ~ 100.0)
	float maxHp;   // 최대 체력 (기본 100.0)
	float 		radianAngle;
	HWND		hWnd;
	HBITMAP		hBitmap;

public:
	Player();
	~Player();
	
	Gdiplus::PointF& GetPos();
	bool Init();
	void Update(float deltaTime);
	void Render(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager);
	void RenderHpBar(Gdiplus::Graphics& g);		
	void DebugRenderCollision(Gdiplus::Graphics& g);	// 충돌영역 디버깅 함수
	void InputProcessing(float deltaTime);
	
	// 플레이어 위치읽기 (Send_Input 에서 사용)
	const Gdiplus::PointF& GetPosition() const { return playerPos; }

	// 플레이어 포지션 변경 (RecvProcess 에서 사용)
	void SetPosition(float x, float y, float rad);

	void LoadPlayerImages(ImageManager& imgManager);
	void SpriteDivideAndRotateRender(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager);
	float CalculateAtan2MouseAtPos(HWND hWnd);

	void ApplyDamage(float amount);
	void SpriteDeathRender(Gdiplus::Graphics& graphics, ImageManager& imgManager); 
	bool IsDead() const { return hp <= 0.0f; }		// 죽었는지 상태 체크
};