#include "Timer.h"

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::unique_ptr;
using std::make_unique;
using std::lock_guard;
using std::mutex;

Timer::Timer()
{
	moveEventPoint = system_clock::now();

	for (bool i : isGrenadeExist) {
		isGrenadeExist[i] = false;
	}
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

	for (int i = 0; i < 3; ++i) {
		if (isGrenadeExist[i] && seconds(3) < duration_cast<seconds>(timePoint - grenadeArray[i])) {
			lock_guard<mutex> lock(timerMutex);
			// ÆøÅº ÅÍÁü.
			isGrenadeExist[i] = false;
			unique_ptr<GameEvent> grenadeExplosion = make_unique<GrenadeExplosion>(i);
			EventQueue::GetInstance().PushEvent(std::move(grenadeExplosion));
		}
	}
}

bool Timer::AddGrenade(int id)
{
	if (isGrenadeExist[id]) {
		printf("ÆøÅºÀÌ ¼³Ä¡µÇ¾î ÀÖ´Â »óÅÂÀÔ´Ï´Ù.\n");
		return false;
	}
	else {
		isGrenadeExist[id] = true;
		grenadeArray[id] = system_clock::now();
		return true;
	}
}