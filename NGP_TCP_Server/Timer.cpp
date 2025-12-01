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
	moveEventCount = system_clock::now();
}

void Timer::TimerLoop()
{
	while (true) {
		timePoint = system_clock::now();
		if (milliseconds(200) < duration_cast<milliseconds>(moveEventCount - timePoint)) {
			lock_guard<mutex> lock(timerMutex);
			moveEventCount = system_clock::now();

			unique_ptr<GameEvent> moveUpdate = make_unique<PlayerUpdate>();
			EventQueue::GetInstance().PushEvent(std::move(moveUpdate));
		}
	}
}