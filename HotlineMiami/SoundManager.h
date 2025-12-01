#pragma once
#include <Windows.h>
#include <string>
#include <vector>

struct IXAudio2;
struct IXAudio2SourceVoice;
struct IXAudio2MasteringVoice;

class SoundManager
{
public:
    struct SoundEntry
    {
        std::string key;          // "grenade_explosion"
        std::string filepath;     // "Resource/Sound/GrenadeExplosion.wav"
        WAVEFORMATEX format{};    // WAV 포맷 정보
        std::vector<BYTE> audioData; // PCM 데이터
        bool loaded = false;      // 파일 로드 여부
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
    void Update();                          // 재생이 끝난 보이스 정리

    // 디버그용
    const SoundEntry* FindSound(const std::string& key) const;

private:
    SoundManager();
    ~SoundManager();

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    SoundEntry* FindSoundInternal(const std::string& key);

private:
    std::vector<SoundEntry> sounds;
    std::string currentBGM;

    IXAudio2* xaudio;
    IXAudio2MasteringVoice* masterVoice;
    IXAudio2SourceVoice* bgmVoice;
    std::vector<IXAudio2SourceVoice*> activeVoices; // 재생 중인 효과음 보이스

    bool comInitialized = false;
};
