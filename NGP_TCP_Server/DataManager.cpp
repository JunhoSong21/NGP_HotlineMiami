#include "DataManager.h"

using std::unique_ptr;
using std::lock_guard;
using std::mutex;

void DataManager::AddPlayer(unique_ptr<Player> player)
{
	lock_guard<mutex> lock(playerMapMutex);

	if (player) {
		playerData[player->playerId] = std::move(player);
		printf("player 추가 완료\n");
	}
	else
		printf("player 데이터 nullptr\n");
}

void DataManager::AddBullet(unique_ptr<Bullet> bullet)
{
	lock_guard<mutex> lock(bulletMapMutex);

	if (bullet) {
		bulletData[bullet->bulletId] = std::move(bullet);
		printf("bullet 추가 완료\n");
	}
	else
		printf("bullet 데이터 nullptr\n");
}

void DataManager::AddGrenade(unique_ptr<Grenade> grenade)
{
	lock_guard<mutex> lock(grenadeMapMutex);

	if (grenade) {
		grenadeData[grenade->grenadeId] = std::move(grenade);
		printf("Grenade 추가 완료\n");
	}
	else
		printf("Grenade 데이터 nullptr\n");
}

Player* DataManager::GetPlayer(int id)
{
	lock_guard<mutex> lock(playerMapMutex);

	auto it = playerData.find(id);
	if (it != playerData.end())
		return it->second.get();
	else {
		printf("GetPlayer() nullptr\n");
		return nullptr;
	}
}

Bullet* DataManager::GetBullet(int id)
{
	lock_guard<mutex> lock(bulletMapMutex);

	auto it = bulletData.find(id);
	if (it != bulletData.end())
		return it->second.get();
	else {
		printf("GetBullet() nullptr\n");
		return nullptr;
	}
}

Grenade* DataManager::GetGrenade(int id)
{
	lock_guard<mutex> lock(grenadeMapMutex);

	auto it = grenadeData.find(id);
	if (it != grenadeData.end())
		return it->second.get();
	else {
		printf("GetGrenade() nullptr\n");
		return nullptr;
	}
}

void DataManager::CollisionCheck()
{

}

bool DataManager::PlayerToBulletCollision()
{
	
}

bool DataManager::PlayerToGrenadeCollision()
{

}