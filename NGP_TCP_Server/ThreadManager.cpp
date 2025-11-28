#include "ThreadManager.h"

using std::lock_guard;
using std::mutex;
using std::unique_ptr;
using std::make_unique;
using std::shared_ptr;

ThreadManager::ThreadManager() :
	threadId(0)
{
}

void ThreadManager::AddThread(SOCKET clientSock)
{
	lock_guard<mutex> lock(threadMutex);

	int clientThreadId = threadId;
	networkThreads[clientThreadId] = make_unique<NetworkThread>(clientThreadId, clientSock);

	//auto newPlayer = std::make_unique<Player>(threadId, threadId);
	auto newPlayer = make_unique<Player>();
	DataManager::GetInstance().AddPlayer(std::move(newPlayer));

	// 클라이언트가 가진 플레이어, 총알, 수류탄 정보를 쉽게 관리하기 위해
	// 쓰레드 id를 공용키로 사용할 수 있도록 한다.
	++threadId;
}

void ThreadManager::RemoveThread(int id)
{
	lock_guard<mutex> lock(threadMutex);

	auto it = networkThreads.find(id);
	if (it != networkThreads.end())
		networkThreads.erase(it);
}

int ThreadManager::ThreadCount()
{
	lock_guard<mutex> lock(threadMutex);
	return networkThreads.size();
}

void ThreadManager::BroadcastEvent(unique_ptr<GameEvent> event)
{
	shared_ptr<GameEvent> sharedEvent = std::move(event);
	for (const auto& pair : networkThreads) {
		pair.second->SendQueueInput(sharedEvent);
	}
}