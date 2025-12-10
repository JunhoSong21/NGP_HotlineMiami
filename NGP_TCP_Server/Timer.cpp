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

	for (bool i : isBulletExist)
		isBulletExist[i] = false;

	for (bool i : isGrenadeExist)
		isGrenadeExist[i] = false;
}

void Timer::TimerLoop()
{
	timePoint = system_clock::now();
	// Player Pos Update
	if (milliseconds(20) < duration_cast<milliseconds>(timePoint - moveEventPoint)) {
		lock_guard<mutex> lock(timerMutex);
		moveEventPoint = system_clock::now();

		unique_ptr<GameEvent> moveUpdate = make_unique<PlayerUpdate>();
		EventQueue::GetInstance().PushEvent(std::move(moveUpdate));
	}

	// Bullet Pos Update
	if (milliseconds(20) < duration_cast<milliseconds>(timePoint - bulletEventPoint)) {
		lock_guard<mutex> lock(timerMutex);
		bulletEventPoint = system_clock::now();

		for (int i = 0; i < 3; ++i) {
			auto bullet = DataManager::GetInstance().GetBullet(i);
			if (bullet->IsActive()) {
				bullet->CalcPosbyAngle();
			}
		}

		unique_ptr<GameEvent> bulletUpdate = make_unique<BulletUpdate>();
		EventQueue::GetInstance().PushEvent(std::move(bulletUpdate));
	}

	DataManager::GetInstance().CollisionCheck();

	// Grenade Time Limit
	for (int i = 0; i < 3; ++i) {
		if (isGrenadeExist[i] && seconds(3) < duration_cast<seconds>(timePoint - grenadeArray[i])) {
			lock_guard<mutex> lock(timerMutex);
			// 폭탄 터짐.
			isGrenadeExist[i] = false;
			unique_ptr<GameEvent> grenadeExplosion = make_unique<GrenadeExplosion>(i);
			EventQueue::GetInstance().PushEvent(std::move(grenadeExplosion));
		}
	}
	
}

bool Timer::AddBullet(int id)
{
	if (isBulletExist[id]) {
		printf("총알이 발사중인 상태입니다.\n");
		return false;
	}
	else {
		isBulletExist[id] = true;
		bulletArray[id] = system_clock::now();
		return true;
	}
}

bool Timer::AddGrenade(int id)
{
	if (isGrenadeExist[id]) {
		printf("폭탄이 설치되어 있는 상태입니다.\n");
		return false;
	}
	else {
		isGrenadeExist[id] = true;
		grenadeArray[id] = system_clock::now();
		return true;
	}
}