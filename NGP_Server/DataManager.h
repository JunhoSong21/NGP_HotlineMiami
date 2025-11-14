#pragma once

class DataManager {
private:
	// 여러 클래스 3개 정의

	DataManager() = default;
	DataManager(const DataManager&) = delete;
	DataManager& operator=(const DataManager&) = delete;

public:
	static DataManager& GetInstance()
	{
		static DataManager instance;
		return instance;
	}

	void PlayerChange(int i);
	void BulletChange(int i);
	void GrenadeChange(int t);
};