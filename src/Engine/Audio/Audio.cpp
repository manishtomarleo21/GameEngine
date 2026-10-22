#include "Engine/Audio/Audio.h"
#include <SDL3/SDL.h>
#include <iostream>

Audio& Audio::Get() {
    static Audio instance;
    return instance;
}

bool Audio::Init() {
    if (m_Initialized) return true;

    // Make sure SDL audio is up
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        std::cerr << "SDL_InitSubSystem(AUDIO) failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!MIX_Init()) {
        std::cerr << "MIX_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    m_Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!m_Mixer) {
        std::cerr << "MIX_CreateMixerDevice failed: " << SDL_GetError() << std::endl;
        return false;
    }

    m_Initialized = true;
    std::cout << "Audio system initialized (SDL3_mixer new API)" << std::endl;
    return true;
}

void Audio::Shutdown() {
    for (auto& [id, audio] : m_Sounds) {
        if (audio) MIX_DestroyAudio(audio);
    }
    m_Sounds.clear();

    for (auto& [id, audio] : m_Music) {
        if (audio) MIX_DestroyAudio(audio);
    }
    m_Music.clear();

    if (m_Mixer) {
        MIX_DestroyMixer(m_Mixer);
        m_Mixer = nullptr;
    }

    if (m_Initialized) {
        MIX_Quit();
        m_Initialized = false;
    }
}

bool Audio::LoadSound(const std::string& id, const std::string& path) {
    if (!m_Mixer) return false;
    if (m_Sounds.count(id)) return true;

    MIX_Audio* audio = MIX_LoadAudio(m_Mixer, path.c_str(), false);
    if (!audio) {
        std::cerr << "Failed to load sound: " << path << " | " << SDL_GetError() << std::endl;
        return false;
    }

    m_Sounds[id] = audio;
    std::cout << "Loaded sound: " << id << std::endl;
    return true;
}

bool Audio::LoadMusic(const std::string& id, const std::string& path) {
    if (!m_Mixer) return false;
    if (m_Music.count(id)) return true;

    MIX_Audio* audio = MIX_LoadAudio(m_Mixer, path.c_str(), false);
    if (!audio) {
        std::cerr << "Failed to load music: " << path << " | " << SDL_GetError() << std::endl;
        return false;
    }

    m_Music[id] = audio;
    std::cout << "Loaded music: " << id << std::endl;
    return true;
}

void Audio::PlaySound(const std::string& id) {
    if (!m_Mixer) return;

    auto it = m_Sounds.find(id);
    if (it == m_Sounds.end()) return;

    // Fire-and-forget sound
    MIX_PlayAudio(m_Mixer, it->second);
}

void Audio::PlayMusic(const std::string& id, bool loop) {
    if (!m_Mixer) return;

    auto it = m_Music.find(id);
    if (it == m_Music.end()) return;

    // Simple approach: create a track and play
    MIX_Track* track = MIX_CreateTrack(m_Mixer);
    if (!track) return;

    MIX_SetTrackAudio(track, it->second);

    SDL_PropertiesID props = SDL_CreateProperties();
    if (loop) {
        SDL_SetNumberProperty(props, "loops", -1); // infinite
    }

    MIX_PlayTrack(track, props);
    SDL_DestroyProperties(props);
}

void Audio::StopMusic() {
    // For now simple: stop all tracks on mixer if available
    // (We can improve this later with a dedicated music track pointer)
}