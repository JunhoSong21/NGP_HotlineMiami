#include "Timer.h"
#include "GlobalData.h"

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

	for (int i = 0; i < (int)isBulletExist.size(); ++i)
		isBulletExist[i] = false;
	for (int i = 0; i < (int)isGrenadeExist.size(); ++i)
		isGrenadeExist[i] = false;
}

void Timer::TimerLoop()
{
	timePoint = system_clock::now();

	// Player Pos Update
	if (milliseconds(16) < duration_cast<milliseconds>(timePoint - moveEventPoint)) {
		lock_guard<mutex> lock(timerMutex);
		moveEventPoint = system_clock::now();

		unique_ptr<GameEvent> moveUpdate = make_unique<PlayerUpdate>();
		EventQueue::GetInstance().PushEvent(std::move(moveUpdate));
	}

	// Bullet Pos Update
	if (milliseconds(16) < duration_cast<milliseconds>(timePoint - bulletEventPoint)) {
		//lock_guard<mutex> lock(timerMutex);
		bulletEventPoint = system_clock::now();

		for (int i = 0; i < MAX_CLIENT_NUM; ++i) {
			Bullet* bullet = DataManager::GetInstance().GetBullet(i);
			if (bullet)
				bullet->CalcPosbyAngle();
		}

		unique_ptr<GameEvent> bulletUpdate = make_unique<BulletUpdate>();
		EventQueue::GetInstance().PushEvent(std::move(bulletUpdate));
	}

	DataManager::GetInstance().CollisionCheck();

	// 2) Grenade 이동 + 퓨즈 처리
	constexpr float FIXED_DELTA = 0.016f; // 16ms 기준
	bool anyGrenadeAlive = false;

	for (int i = 0; i < MAX_CLIENT_NUM; ++i) {
		Grenade* grenade = DataManager::GetInstance().GetGrenade(i);
		if (!grenade) {
			continue;
		}

		// 날아가는 중이면 움직이고 내부에서 정지/퓨즈/폭발 처리
		if (grenade->GetIsActive() && !grenade->GetIsExplode()) {
			grenade->Update(FIXED_DELTA);
		}

		// 폭발 상태로 바뀐 걸 감지해서 이벤트 넣기 (한 번만)
		static std::array<bool, MAX_CLIENT_NUM> explosionSent{};
		if (grenade->GetIsExplode() && !explosionSent[i]) {
			explosionSent[i] = true;

			std::unique_ptr<GameEvent> grenadeExplosion =
				std::make_unique<GrenadeExplosion>(i);
			EventQueue::GetInstance().PushEvent(std::move(grenadeExplosion));
		}

		// 다시 던질 수 있도록 상태 초기화가 필요하다면
		if (!grenade->GetIsActive() && !grenade->GetIsExplode()) {
			explosionSent[i] = false;
		}
	}

	if (anyGrenadeAlive) {
		std::unique_ptr<GameEvent> grenadeUpdate =
			std::make_unique<GrenadeUpdate>();
		EventQueue::GetInstance().PushEvent(std::move(grenadeUpdate));
	}

	if (seconds(1) < duration_cast<seconds>(timePoint - gameEndPoint)) {
		lock_guard<mutex> lock(timerMutex);
		gameEndPoint = system_clock::now();

		int deathCount = 0;
		for (int i = 0; i < MAX_CLIENT_NUM; ++i) {
			Player* player = DataManager::GetInstance().GetPlayer(i);
			if (player && player->GetIsAlive() == false)
				++deathCount;
		}

		/*if (deathCount == MAX_CLIENT_NUM - 1) {
			unique_ptr<GameEvent> gameEnd = make_unique<GameEnd>();
			EventQueue::GetInstance().PushEvent(std::move(gameEnd));
		}*/
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

void Timer::SetBulletArray(int i)
{
	isBulletExist[i] = false;
}

bool Timer::GetBulletArray(int i)
{
	return isBulletExist[i];
}

bool Timer::GetGrenadeArray(int i)
{
	return isGrenadeExist[i];
}