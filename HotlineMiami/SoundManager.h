#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <vector>

#pragma comment(lib, "winmm.lib")

class SoundManager
{
public:
    struct SoundEntry
    {
        std::string key;       // "grenade_explosion"
        std::string filepath;  // "Resource/Sound/GrenadeExplosion.wav"
    };

    static SoundManager& Get()
    {
        static SoundManager instance;
        return instance;
    }

    void LoadSound(const std::string& key, const std::string& filePath);
    void Play(const std::string& key);      // 효과음 1회 재생
    void PlayBGM(const std::string& key);   // BGM 루프 재생
    void StopBGM();                         // BGM 정지
    void StopAll();                         // 모든 사운드 정지

private:
    SoundManager() {}
    ~SoundManager() {}

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    // 내부 검색용 헬퍼 함수
    const SoundEntry* FindSound(const std::string& key) const;

private:
    std::vector<SoundEntry> sounds;
    std::string currentBGM;
};

