#pragma once

class Grenade {
public:
	int grenadeId;

private:
	bool	isActive;	// 투척 여부
	bool	isExplode;	// 폭발 여부
	float velX;
	float velY;
	float	posX;
	float	posY;
	bool  hasStopped;    // 속도가 거의 0이 되어서 멈춘 상태
	bool  fuseStarted;   // 정지 후 퓨즈 ON 여부
	float fuseElapsed;   // 퓨즈 켜진 후 경과 시간(초)

public:
	Grenade(int id);

	void Throw(float startX, float startY, float dirRadAngle);
	void Update(float deltaSeconds);

	void ChangeStateToExplode();
	void ChangeStateToDebris();

	void SetIsActive(bool active);
	void SetPos(float x, float y);

	float GetPosX() const { return posX; }
	float GetPosY() const { return posY; }

	bool GetIsActive() const { return isActive; }
	bool GetIsExplode() const { return isExplode; }

	bool HasStopped() const { return hasStopped; }
	bool IsFuseStarted() const { return fuseStarted; }
	float GetRemainFuse() const;
};