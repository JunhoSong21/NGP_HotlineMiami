#include "Timer.h"

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::unique_ptr;
using std::make_unique;
using std::lock_guard;
using std::mutex;

Timer::Timer()
{
	moveEventPoint = system_clock::now();
}

void Timer::TimerLoop()
{
	timePoint = system_clock::now();
	if (milliseconds(200) < duration_cast<milliseconds>(timePoint - moveEventPoint)) {
		lock_guard<mutex> lock(timerMutex);
		moveEventPoint = system_clock::now();

		unique_ptr<GameEvent> moveUpdate = make_unique<PlayerUpdate>();
		EventQueue::GetInstance().PushEvent(std::move(moveUpdate));
	}
}