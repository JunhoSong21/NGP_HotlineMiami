#pragma once
#include <memory>

class GameEvent;
class EventManager;
class ThreadManager;

class UpdateProcess {
public:
	UpdateProcess();
	~UpdateProcess();

	void EventProcess();

private:
	void ProcessPlayerInput();
	void ProcessBulletInput();
	void ProcessGrenadeInput();

	void TimerEvent();
};

