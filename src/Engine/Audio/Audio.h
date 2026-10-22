#pragma once

#include <SDL3_mixer/SDL_mixer.h>
#include <string>
#include <unordered_map>


class Audio {
public:
    static Audio& Get();

    bool Init();
    void Shutdown();

    bool LoadSound(const std::string& id, const std::string& path);
    bool LoadMusic(const std::string& id, const std::string& path);

    void PlaySound(const std::string& id);
    void PlayMusic(const std::string& id, bool loop = true);

    void StopMusic();

private:
    Audio() = default;
    ~Audio() = default;

    bool m_Initialized = false;
    MIX_Mixer* m_Mixer = nullptr;

    std::unordered_map<std::string, MIX_Audio*> m_Sounds;
    std::unordered_map<std::string, MIX_Audio*> m_Music;
};