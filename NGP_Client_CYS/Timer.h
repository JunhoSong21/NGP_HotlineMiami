#pragma once

class Timer {
private:
	LARGE_INTEGER frequency;	// 초당 카운트 횟수
	LARGE_INTEGER lastTime;		// 이전 프레임 시간

public:
	Timer();
	~Timer();

	float getDeltaTime();
};