#pragma once

#include "Engine/Renderer/Texture.h"
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
#include <memory>

class TextureManager {
    public:
        static TextureManager& Get();   //Singleton

        std::shared_ptr<Texture> Load(SDL_Renderer* renderer, const std::string& path);
        void Clear();

    private:
        TextureManager() = default;
        ~TextureManager() = default;

        // Prevent copying
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator = (const TextureManager&) = delete;
        
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
};